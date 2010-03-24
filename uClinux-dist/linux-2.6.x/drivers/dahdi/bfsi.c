/*
  bfsi.c
  David Rowe 21 June 2006

  Functions for Linux device drivers on the Blackfin that
  support interfacing the Blackfin to Silicon Labs chips.

  These functions are in a separate file from the target wcfxs driver
  so they can be re-used with different drivers, for example unit
  test software.

  For various reasons the CPHA=1 (sofware controlled SPISEL)
  mode needs to be used, for example the SiLabs chip expects
  SPISEL to go high between 8 bit transfers and the timing
  the Si3050 expects (Figs 3 & 38 of 3050 data sheet) are
  closest to Fig 10-12 of the BF533 hardware reference manual.

  See also unittest/spi for several unit tests.
*/

/*
  Copyright (C) 2006 David Rowe

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/bfin5xx_spi.h>
#include <linux/delay.h>
#include <asm/dma.h>
#include <asm/irq.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>

/* Default for BF537 STAMP is SPORT1 */

#if defined(CONFIG_BF537)
#define BFSI_SPORT1
#endif

#define BFIN_IP01

#undef BFIN_SPI_DEBUG

#ifdef BFIN_SPI_DEBUG
#define PRINTK(args...) printk(args)
#else
#define PRINTK(args...)
#endif

/*
   I found these macros from the bfin5xx_spi.c driver by Luke Yang
   useful - thanks Luke :-)
*/

#define DEFINE_SPI_REG(reg, off) \
static inline u16 read_##reg(void) \
            { return *(volatile unsigned short*)(SPI0_REGBASE + off); } \
static inline void write_##reg(u16 v) \
            {*(volatile unsigned short*)(SPI0_REGBASE + off) = v;\
             __builtin_bfin_ssync();}

DEFINE_SPI_REG(CTRL, 0x00)
DEFINE_SPI_REG(FLAG, 0x04)
DEFINE_SPI_REG(STAT, 0x08)
DEFINE_SPI_REG(TDBR, 0x0C)
DEFINE_SPI_REG(RDBR, 0x10)
DEFINE_SPI_REG(BAUD, 0x14)
DEFINE_SPI_REG(SHAW, 0x18)

/* constants for isr cycle averaging */

#define TC    1024 /* time constant    */
#define LTC   10   /* base 2 log of TC */

/* ping-pong buffer states for debug */

#define PING 0
#define PONG 1

/* misc statics */

static u8 *iTxBuffer1;
static u8 *iRxBuffer1;

static int samples_per_chunk;
static int row_length;
static int internalclock = 0;
static int bfsi_debug = 0;
static int bfsi_widebandmode = 0;
static int init_ok = 0;
static u16 chip_select_mask = 0;

/* isr callback installed by user */

static void (*bfsi_isr_callback)(u8 *read_samples, u8 *write_samples) = NULL;

/* debug variables */

static int readchunk_first = 0;
static int readchunk_second = 0;
static int readchunk_didntswap = 0;
static int lastreadpingpong;

static int bad_xr[5];
static int last_xr[5];
static int bad_xw[5];
static int last_xw[5];
static u8 *log_readchunk;

static int writechunk_first = 0;
static int writechunk_second = 0;
static int writechunk_didntswap = 0;
static int lastwritepingpong;

/* previous and worst case number of cycles we took to process an
   interrupt */

static u32 isr_cycles_last = 0;
static u32 isr_cycles_worst = 0;
static u32 isr_cycles_average = 0; /* scaled up by 2x */
static u32 echo_sams = 0;

/* monitor cycles between ISRs */

static u32 isr_between_prev = 0;
static u32 isr_between_worst = 0;
static u32 isr_between_diff = 0;
static u32 isr_between_difflastskip = 0;
static u32 isr_between_skip = 0;

/* freeze ISR activity for test purposes */

static int bfsi_freeze = 0;

/* sample cycles register of Blackfin */

static inline unsigned int cycles(void) {
  int ret;

   __asm__ __volatile__
   (
   "%0 = CYCLES;\n\t"
   : "=&d" (ret)
   :
   : "R1"
   );

   return ret;
}

/*------------------------- SPI FUNCTIONS -----------------------------*/

/*
   After much experimentation I found that (i) TIMOD=00 (i.e. using
   read_RDBR() to start transfer) was the best way to start transfers
   and (ii) polling RXS was the best way to end transfers, see p10-30
   and p10-31 of BF533 data book.

   chip_select is the _number_ of the chip select line, e.g. to use
   SPISEL2 chip_select = 2.
*/

void bfsi_spi_write_8_bits(u16 chip_select, u8 bits)
{
  u16 flag_enable, flag;

  if (chip_select < 8) {
    flag = read_FLAG();
    flag_enable = flag & ~(1 << (chip_select + 8));
    PRINTK("chip_select: %d write: flag: 0x%04x flag_enable: 0x%04x \n",
	   chip_select, flag, flag_enable);

    /* drop SPISEL */
    write_FLAG(flag_enable);
  }
  else {
  	bfin_write_FIO_FLAG_C((1<<chip_select));
  	__builtin_bfin_ssync();
  }

  /* read kicks off transfer, detect end by polling RXS */
  write_TDBR(bits);
  read_RDBR(); __builtin_bfin_ssync();
  do {} while (!(read_STAT() & RXS) );

  /* raise SPISEL */
  if (chip_select < 8) {
    write_FLAG(flag);
  }
  else {
    bfin_write_FIO_FLAG_S((1<<chip_select));
    __builtin_bfin_ssync();
  }
}

u8 bfsi_spi_read_8_bits(u16 chip_select)
{
  u16 flag_enable, flag, ret;

  if (chip_select < 8) {
    flag = read_FLAG();
    flag_enable = flag & ~(1 << (chip_select + 8));
    PRINTK("read: flag: 0x%04x flag_enable: 0x%04x \n",
	   flag, flag_enable);
  }
  else {
    bfin_write_FIO_FLAG_C((1<<chip_select));
    __builtin_bfin_ssync();
  }

  /* drop SPISEL */
  write_FLAG(flag_enable);

  /* read kicks off transfer, detect end by polling RXS, we
     read the shadow register to prevent another transfer
     being started

     While reading we write a dummy tx value, 0xff.  For
     the MMC card, a 0 bit indicates the start of a command
     sequence therefore an all 1's sequence keeps the MMC
     card in the current state.
  */
  write_TDBR(0xff);
  read_RDBR(); __builtin_bfin_ssync();
  do {} while (!(read_STAT() & RXS) );
  ret = bfin_read_SPI_SHADOW();

  /* raise SPISEL */
  if (chip_select < 8) {
    write_FLAG(flag);
  }
  else {
    bfin_write_FIO_FLAG_S((1<<chip_select));
    __builtin_bfin_ssync();
  }

  return ret;
}

/*
   new_chip_select_mask: the logical OR of all the chip selects we wish
   to use for SPI, for example if we wish to use SPISEL2 and SPISEL3
   chip_select_mask = (1<<2) | (1<<3).

   baud:  The SPI clk divider value, see Blackfin Hardware data book,
   maximum speed when baud = 2, minimum when baud = 0xffff (0 & 1
   disable SPI port).

   The maximum SPI clk for the Si Labs 3050 is 16.4MHz.  On a
   100MHz system clock Blackfin this means baud=4 minimum (12.5MHz).

   For the IP04 some extra code needed to be added to the three SPI
   routines to handle the use of PF12 as nCSB.  It's starting to
   look a bit messy and is perhaps inefficient.
*/

void bfsi_spi_init(int baud, u16 new_chip_select_mask)
{
	u16 ctl_reg, flag;
	int cs, bit;

  	if (baud < 4) {
    		printk("baud = %d may mean SPI clock too fast for Si labs 3050"
	   		"consider baud == 4 or greater", baud);
  	}

	PRINTK("bfsi_spi_init\n");
	PRINTK("  new_chip_select_mask = 0x%04x\n", new_chip_select_mask);
	PRINTK("  FIOD_DIR = 0x%04x\n", bfin_read_FIO_DIR());

	/* grab SPISEL/GPIO pins for SPI, keep level of SPISEL pins H */
	chip_select_mask |= new_chip_select_mask;

	flag = 0xff00 | (chip_select_mask & 0xff);

	/* set up chip selects greater than PF7 */

  	if (chip_select_mask & 0xff00) {
	  bfin_write_FIO_DIR(bfin_read_FIO_DIR() | (chip_select_mask & 0xff00));
   	  __builtin_bfin_ssync();
	}
	PRINTK("  After FIOD_DIR = 0x%04x\n", bfin_read_FIO_DIR());

#if defined(CONFIG_BF537)

	/* we need to work thru each bit in mask and set the MUX regs */

	for(bit=0; bit<8; bit++) {
	  if (chip_select_mask & (1<<bit)) {
	    PRINTK("SPI CS bit: %d enabled\n", bit);
	    cs = bit;
	    if (cs == 1) {
	      PRINTK("set for chip select 1\n");
	      bfin_write_PORTF_FER(bfin_read_PORTF_FER() | 0x3c00);
	      __builtin_bfin_ssync();

	    } else if (cs == 2 || cs == 3) {
	      PRINTK("set for chip select 2\n");
	      bfin_write_PORT_MUX(bfin_read_PORT_MUX() | PJSE_SPI);
	      __builtin_bfin_ssync();
	      bfin_write_PORTF_FER(bfin_read_PORTF_FER() | 0x3800);
	      __builtin_bfin_ssync();

	    } else if (cs == 4) {
	      bfin_write_PORT_MUX(bfin_read_PORT_MUX() | PFS4E_SPI);
	      __builtin_bfin_ssync();
	      bfin_write_PORTF_FER(bfin_read_PORTF_FER() | 0x3840);
	      __builtin_bfin_ssync();

	    } else if (cs == 5) {
	      bfin_write_PORT_MUX(bfin_read_PORT_MUX() | PFS5E_SPI);
	      __builtin_bfin_ssync();
	      bfin_write_PORTF_FER(bfin_read_PORTF_FER() | 0x3820);
	      __builtin_bfin_ssync();

	    } else if (cs == 6) {
	      bfin_write_PORT_MUX(bfin_read_PORT_MUX() | PFS6E_SPI);
	      __builtin_bfin_ssync();
	      bfin_write_PORTF_FER(bfin_read_PORTF_FER() | 0x3810);
	      __builtin_bfin_ssync();

	    } else if (cs == 7) {
	      bfin_write_PORT_MUX(bfin_read_PORT_MUX() | PJCE_SPI);
	      __builtin_bfin_ssync();
	      bfin_write_PORTF_FER(bfin_read_PORTF_FER() | 0x3800);
	      __builtin_bfin_ssync();
	    }
	  }
	}
#endif

  	/* note TIMOD = 00 - reading SPI_RDBR kicks off transfer */
  	ctl_reg = SPE | MSTR | CPOL | CPHA | SZ;
  	write_FLAG(flag);
  	write_BAUD(baud);
  	write_CTRL(ctl_reg);
}

/*-------------------------- RESET FUNCTION ----------------------------*/

void bfsi_reset(int reset_bit) {
	PRINTK("toggle reset\n");

#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
       	PRINTK("set reset to PF%d\n",reset_bit);
  	bfin_write_FIO_DIR(bfin_read_FIO_DIR() | (1<<reset_bit));
  	__builtin_bfin_ssync();

  	bfin_write_FIO_FLAG_C((1<<reset_bit));
  	__builtin_bfin_ssync();
  	udelay(100);

  	bfin_write_FIO_FLAG_S((1<<reset_bit));
  	__builtin_bfin_ssync();
#endif

#if defined(CONFIG_BF537)
	if (reset_bit == 1) {
       		PRINTK("set reset to PF10\n");
                bfin_write_PORTF_FER(bfin_read_PORTF_FER() & 0xFBFF);
		__builtin_bfin_ssync();
		bfin_write_PORTFIO_DIR(bfin_read_PORTFIO_DIR() | 0x0400);
		__builtin_bfin_ssync();
		bfin_write_PORTFIO_CLEAR(1<<10);
		__builtin_bfin_ssync();
		udelay(100);
		bfin_write_PORTFIO_SET(1<<10);
		__builtin_bfin_ssync();
        } else if (reset_bit == 2)  {
                PRINTK("Error: cannot set reset to PJ11\n");
        } else if (reset_bit == 3) {
                PRINTK("Error: cannot set reset to PJ10\n");
        } else if (reset_bit == 4) {
                PRINTK("set reset to PF6\n");
                bfin_write_PORTF_FER(bfin_read_PORTF_FER() & 0xFFBF);
                __builtin_bfin_ssync();
		bfin_write_PORTFIO_DIR(bfin_read_PORTFIO_DIR() | 0x0040);
		__builtin_bfin_ssync();
		bfin_write_PORTFIO_CLEAR(1<<6);
		__builtin_bfin_ssync();
		udelay(100);
		bfin_write_PORTFIO_SET(1<<6);
		__builtin_bfin_ssync();
        } else if (reset_bit == 5) {
                PRINTK("set reset to PF5\n");
                bfin_write_PORTF_FER(bfin_read_PORTF_FER() & 0xFFDF);
                __builtin_bfin_ssync();
		bfin_write_PORTFIO_DIR(bfin_read_PORTFIO_DIR() | 0x0020);
		__builtin_bfin_ssync();
		bfin_write_PORTFIO_CLEAR(1<<5);
		__builtin_bfin_ssync();
		udelay(100);
		bfin_write_PORTFIO_SET(1<<5);
		__builtin_bfin_ssync();
        } else if (reset_bit == 6) {
                PRINTK("set reset to PF4\n");
                bfin_write_PORTF_FER(bfin_read_PORTF_FER() & 0xFFEF);
                __builtin_bfin_ssync();
		bfin_write_PORTFIO_DIR(bfin_read_PORTFIO_DIR() | 0x0010);
		__builtin_bfin_ssync();
		bfin_write_PORTFIO_CLEAR(1<<4);
		__builtin_bfin_ssync();
		udelay(100);
		bfin_write_PORTFIO_SET(1<<4);
		__builtin_bfin_ssync();
        } else if (reset_bit == 7) {
                PRINTK("Error: cannot set reset to PJ5\n");
        }
#endif
  /*
     p24 3050 data sheet, allow 1ms for PLL lock, with
     less than 1ms (1000us) I found register 2 would have
     a value of 0 rather than 3, indicating a bad reset.
  */
  udelay(1000);
}

/*-------------------------- SPORT FUNCTIONS ----------------------------*/

/* Init serial port but dont enable just yet, we need to set up DMA first */

/* Note SPORT0 is used for the BF533 STAMP and SPORT1 for the BF537 due to
   the physical alignment of the 4fx cards on the STAMP boards. */

/* Note a better way to write init code that works for both sports is
   in uClinux-dist /linux-2.6.x/sound/blackfin/bf53x_sport.c.  A
   structure is set up with the SPORT register addresses referenced to
   the base ptr of the structure.  This means one function can be used
   to init both SPORTs, just by changing the base addr of the ptr. */

#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
static void init_sport0(void)
{
	/* set up FSYNC and optionally SCLK using Blackfin Serial port */


	u32 sclk = get_sclk();
	u32 pclk =2048000;
	u32 clkdiv= sclk /(2* pclk) - 1;
	u32 rfsdiv=(pclk/8000) -1;
	printk ("System Clock: %d HZ; Clock Divisor: %d RFS Divisor: %d\n", sclk,clkdiv,rfsdiv);

	switch (bfsi_widebandmode)
	{
	case 1:
		bfin_write_SPORT0_TCLKDIV(0);		/* TCLK disabled using external oscillator */
		bfin_write_SPORT0_TFSDIV(0);		/* TCLK disabled*/
		bfin_write_SPORT0_RCLKDIV(0);  		/* RCLK disabled using external oscillator*/
		bfin_write_SPORT0_RFSDIV(rfsdiv);  	/* generates 8 kHz FSYNC */
		bfin_write_SPORT0_TCR2(15);     	/* 16 bit word length      */
		bfin_write_SPORT0_TCR1(0);
		bfin_write_SPORT0_RCR2(15);     	/* 16 bit word length      */
		bfin_write_SPORT0_MCMC1(0x0000);	/* MCM window size of 32 with 0 offset             */
		bfin_write_SPORT0_RCR1(IRFS);
#ifndef BFIN_IP01
		/* Enable MCM 8 transmit & receive channels       */
		bfin_write_SPORT0_MTCS0(0x000000FF);
		bfin_write_SPORT0_MRCS0(0x000000FF);
		/* 0 bit delay between FS pulse and first data bit,
		multichannel frame mode enabled,
		multichannel tx and rx DMA packing enabled */
		bfin_write_SPORT0_MCMC2(0x001c);
#else

		bfin_write_SPORT0_MTCS0(0x00000011);	/* Enable timeslots 0,1 and timeslots 16,17 (timeslot 16 is 62.5us from timeslot 0)*/
		bfin_write_SPORT0_MRCS0(0x00000011);	/* Enable timeslots 0,1 and timeslots 16,17 (timeslot 16 is 62.5us from timeslot 0)*/

		/* 0 bit delay between FS pulse and first data bit,
		multichannel frame mode enabled,
		multichannel tx and rx DMA packing enabled */
		bfin_write_SPORT0_MCMC2(0x001c);

#endif
		break;
	case 2:
		bfin_write_SPORT0_TCLKDIV(0);		/* TCLK disabled using external oscillator */
		bfin_write_SPORT0_TFSDIV(0);		/* TCLK disabled*/
		bfin_write_SPORT0_RCLKDIV(0);  		/* RCLK disabled using external oscillator*/
		bfin_write_SPORT0_RFSDIV(rfsdiv);  	/* generates 8 kHz FSYNC */
		bfin_write_SPORT0_TCR2(15);     	/* 16 bit word length      */
		bfin_write_SPORT0_TCR1(0);
		bfin_write_SPORT0_RCR2(15);     	/* 16 bit word length      */
		bfin_write_SPORT0_MCMC1(0x0000);	/* MCM window size of 32 with 0 offset             */
		bfin_write_SPORT0_RCR1(IRFS);
#ifndef BFIN_IP01
		/* Enable MCM 8 transmit & receive channels       */
		bfin_write_SPORT0_MTCS0(0x000000FF);
		bfin_write_SPORT0_MRCS0(0x000000FF);
		/* MCM window size of 8 with 0 offset             */
		bfin_write_SPORT0_MCMC1(0x0000);

		/* 0 bit delay between FS pulse and first data bit,
		multichannel frame mode enabled,
		multichannel tx and rx DMA packing enabled */
		bfin_write_SPORT0_MCMC2(0x001c);
#else

		bfin_write_SPORT0_MTCS0(0x00000001);	/* Enable timeslots 0,1 */
		bfin_write_SPORT0_MRCS0(0x00000001);	/* Enable timeslots 0,1 */

		/* MCM window size of 16 with 0 offset             */
		//bfin_write_SPORT0_MCMC1(0x0000);
		/* 0 bit delay between FS pulse and first data bit,
		multichannel frame mode enabled,
		multichannel tx and rx DMA packing enabled */
		bfin_write_SPORT0_MCMC2(0x001c);

#endif
		break;
	default:
		bfin_write_SPORT0_TCLKDIV(0);		/* TCLK disabled using external oscillator */
		bfin_write_SPORT0_TFSDIV(0);		/* TCLK disabled*/
		bfin_write_SPORT0_RCLKDIV(0);  		/* RCLK disabled using external oscillator*/
		bfin_write_SPORT0_RFSDIV(rfsdiv);  	/* generates 8 kHz FSYNC */
		bfin_write_SPORT0_TCR2(7);     		/* 8 bit word length      */
		bfin_write_SPORT0_TCR1(0);
		bfin_write_SPORT0_RCR2(7);     		/* 8 bit word length      */
		bfin_write_SPORT0_MCMC1(0x0000);	/* MCM window size of 32 with 0 offset             */
		bfin_write_SPORT0_RCR1(IRFS);
#ifndef BFIN_IP01
		/* Enable MCM 8 transmit & receive channels       */
		bfin_write_SPORT0_MTCS0(0x000000FF);
		bfin_write_SPORT0_MRCS0(0x000000FF);
		/* 0 bit delay between FS pulse and first data bit,
		multichannel frame mode enabled,
		multichannel tx and rx DMA packing enabled */
		bfin_write_SPORT0_MCMC2(0x001c);
#else

		bfin_write_SPORT0_MTCS0(0x00000001);	/* Enable timeslot 0 */
		bfin_write_SPORT0_MRCS0(0x00000001);	/* Enable timeslot 0 */
		/* MCM window size of 16 with 0 offset             */
		//bfin_write_SPORT0_MCMC1(0x0000);
		/* 0 bit delay between FS pulse and first data bit,
		multichannel frame mode enabled,
		multichannel tx and rx DMA packing enabled */
		bfin_write_SPORT0_MCMC2(0x001c);

#endif
		break;
	}





}
#endif

#if defined(CONFIG_BF537)
static void init_sport1(void)
{

        /* BF537 specific pin muxing configuration */
        bfin_write_PORT_MUX(bfin_read_PORT_MUX() | PGTE|PGRE|PGSE);
        __builtin_bfin_ssync();
        bfin_write_PORTG_FER(bfin_read_PORTG_FER() | 0xFF00);
        __builtin_bfin_ssync();

	if (internalclock) {
		bfin_write_SPORT1_RCLKDIV(24);
		bfin_write_SPORT1_RFSDIV(255);
	}
	else {
		bfin_write_SPORT1_RFSDIV(255);
	}

	bfin_write_SPORT1_TCR2(7);      /* 8 bit word length      */
	bfin_write_SPORT1_TCR1(0);

	/* rx enabled, MSB first, internal frame sync     */
	bfin_write_SPORT1_RCR2(7);      /* 8 bit word length      */
	if (internalclock) {
		bfin_write_SPORT1_RCR1(IRFS | IRCLK);
	}
	else {
		bfin_write_SPORT1_RCR1(IRFS);
	}

	/* Enable MCM 8 transmit & receive channels       */
	bfin_write_SPORT1_MTCS0(0x000000FF);
	bfin_write_SPORT1_MRCS0(0x000000FF);

	/* MCM window size of 8 with 0 offset             */
	bfin_write_SPORT1_MCMC1(0x0000);

	/* 0 bit delay between FS pulse and first data bit,
	   multichannel frame mode enabled,
	   multichannel tx and rx DMA packing enabled */
	bfin_write_SPORT1_MCMC2(0x001c);
}
#endif

/* init DMA for autobuffer mode, but dont enable yet */

static void init_dma_wc(void)
{
#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
  /* Set up DMA1 to receive, map DMA1 to Sport0 RX */
  bfin_write_DMA1_PERIPHERAL_MAP(0x1000);
  bfin_write_DMA1_IRQ_STATUS(bfin_read_DMA1_IRQ_STATUS() | 0x2);
#endif
#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
  /* Set up DMA3 to receive, map DMA3 to Sport0 RX */
  bfin_write_DMA3_PERIPHERAL_MAP(0x3000);
  bfin_write_DMA3_IRQ_STATUS(bfin_read_DMA3_IRQ_STATUS() | 0x2);
#endif
#if defined(BFSI_SPORT1)
  /* Set up DMA5 to receive, map DMA5 to Sport1 RX */
  bfin_write_DMA5_PERIPHERAL_MAP(0x5000);
  bfin_write_DMA5_IRQ_STATUS(bfin_read_DMA5_IRQ_STATUS() | 0x2);
#endif
#endif

#if L1_DATA_A_LENGTH != 0
#ifndef BFIN_IP01
  iRxBuffer1 = (char*)l1_data_A_sram_alloc(2*samples_per_chunk*8);
#else
	  iRxBuffer1 = (char*)l1_data_A_sram_alloc(2*samples_per_chunk);
#endif
#else
  {
    dma_addr_t addr;
    iRxBuffer1 = (char*)dma_alloc_coherent(NULL, 2*samples_per_chunk*8, &addr, 0);
  }
#endif
  if (bfsi_debug)
    printk("iRxBuffer1 = 0x%x\n", (int)iRxBuffer1);

#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
  /* Start address of data buffer */
  bfin_write_DMA1_START_ADDR(iRxBuffer1);

  /* DMA inner loop count */
#ifndef BFIN_IP01
  bfin_write_DMA1_X_COUNT(samples_per_chunk*8);
  /* Inner loop address increment */
  bfin_write_DMA1_X_MODIFY(1);
  bfin_write_DMA1_Y_MODIFY(1);
  bfin_write_DMA1_Y_COUNT(2);

  /* Configure DMA1
     8-bit transfers, Interrupt on completion, Autobuffer mode */
  bfin_write_DMA1_CONFIG(WNR | WDSIZE_8 | DI_EN | 0x1000 | DI_SEL | DMA2D);

  /* Set up DMA2 to transmit, map DMA2 to Sport0 TX */
  bfin_write_DMA2_PERIPHERAL_MAP(0x2000);
  /* Configure DMA2 8-bit transfers, Autobuffer mode */
  bfin_write_DMA2_CONFIG(WDSIZE_8 | 0x1000 | DMA2D);

#else

	switch (bfsi_widebandmode)
	{
	case 1:
		bfin_write_DMA1_X_COUNT(samples_per_chunk/2);
		/* Inner loop address increment */
		bfin_write_DMA1_X_MODIFY(2);
		bfin_write_DMA1_Y_MODIFY(2);
		bfin_write_DMA1_Y_COUNT(2);
		/* Configure DMA1
		8-bit transfers, Interrupt on completion, Autobuffer mode */
		bfin_write_DMA1_CONFIG(WNR | WDSIZE_16 | DI_EN | DMAFLOW_AUTO | DI_SEL | DMA2D);
		break;
	case 2:
		bfin_write_DMA1_X_COUNT(samples_per_chunk/2);
		/* Inner loop address increment */
		bfin_write_DMA1_X_MODIFY(2);
		bfin_write_DMA1_Y_MODIFY(2);
		bfin_write_DMA1_Y_COUNT(2);
		/* Configure DMA1
		8-bit transfers, Interrupt on completion, Autobuffer mode */
		bfin_write_DMA1_CONFIG(WNR | WDSIZE_16 | DI_EN | DMAFLOW_AUTO | DI_SEL | DMA2D);
		break;
	default:
		bfin_write_DMA1_X_COUNT(samples_per_chunk);
		/* Inner loop address increment */
		bfin_write_DMA1_X_MODIFY(1);
		bfin_write_DMA1_Y_MODIFY(1);
		bfin_write_DMA1_Y_COUNT(2);
		/* Configure DMA1
		8-bit transfers, Interrupt on completion, Autobuffer mode */
		bfin_write_DMA1_CONFIG(WNR | WDSIZE_8 | DI_EN | DMAFLOW_AUTO | DI_SEL | DMA2D);
		break;
	}

  /* Set up DMA2 to transmit, map DMA2 to Sport0 TX */
  bfin_write_DMA2_PERIPHERAL_MAP(0x2000);
  /* Configure DMA2 8-bit transfers, Autobuffer mode */
  switch (bfsi_widebandmode)
  	{
  	case 1:
  		bfin_write_DMA2_CONFIG(WDSIZE_16 | DMAFLOW_AUTO | DMA2D);
  		break;
  	case 2:
		bfin_write_DMA2_CONFIG(WDSIZE_16 | DMAFLOW_AUTO | DMA2D);
		break;
  	default:
		bfin_write_DMA2_CONFIG(WDSIZE_8 | DMAFLOW_AUTO | DMA2D);
		break;
  	}

#endif


#endif

#if defined(CONFIG_BF537)

#if defined(BFSI_SPORT0)
  /* Start address of data buffer */
  bfin_write_DMA3_START_ADDR(iRxBuffer1);

  /* DMA inner loop count */
  bfin_write_DMA3_X_COUNT(samples_per_chunk*8);

  /* Inner loop address increment */
  bfin_write_DMA3_X_MODIFY(1);
  bfin_write_DMA3_Y_MODIFY(1);
  bfin_write_DMA3_Y_COUNT(2);

  /* Configure DMA3
     8-bit transfers, Interrupt on completion, Autobuffer mode */
  bfin_write_DMA3_CONFIG(WNR | WDSIZE_8 | DI_EN | 0x1000 | DI_SEL | DMA2D);
  /* Set up DMA4 to transmit, map DMA4 to Sport0 TX */
  bfin_write_DMA4_PERIPHERAL_MAP(0x4000);
  /* Configure DMA4 8-bit transfers, Autobuffer mode */
  bfin_write_DMA4_CONFIG(WDSIZE_8 | 0x1000 | DMA2D);
#endif

#if defined(BFSI_SPORT1)
  /* Start address of data buffer */
  bfin_write_DMA5_START_ADDR(iRxBuffer1);

  /* DMA inner loop count */
  bfin_write_DMA5_X_COUNT(samples_per_chunk*8);

  /* Inner loop address increment */
  bfin_write_DMA5_X_MODIFY(1);
  bfin_write_DMA5_Y_MODIFY(1);
  bfin_write_DMA5_Y_COUNT(2);

  /* Configure DMA5
     8-bit transfers, Interrupt on completion, Autobuffer mode */
  bfin_write_DMA5_CONFIG(WNR | WDSIZE_8 | DI_EN | 0x1000 | DI_SEL | DMA2D);
  /* Set up DMA6 to transmit, map DMA6 to Sport1 TX */
  bfin_write_DMA6_PERIPHERAL_MAP(0x6000);
  /* Configure DMA2 8-bit transfers, Autobuffer mode */
  bfin_write_DMA6_CONFIG(WDSIZE_8 | 0x1000 | DMA2D);
#endif
#endif

#if L1_DATA_A_LENGTH != 0
#ifndef BFIN_IP01
  iTxBuffer1 = (char*)l1_data_A_sram_alloc(2*samples_per_chunk*8);
#else
	  iTxBuffer1 = (char*)l1_data_A_sram_alloc(2*samples_per_chunk);
#endif
#else
  {
    dma_addr_t addr;
    iTxBuffer1 = (char*)dma_alloc_coherent(NULL, 2*samples_per_chunk*8, &addr, 0);
  }
#endif
  if (bfsi_debug)
    printk("iTxBuffer1 = 0x%x\n", (int)iTxBuffer1);

#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
  /* Start address of data buffer */
  bfin_write_DMA2_START_ADDR(iTxBuffer1);

  /* DMA inner loop count */
#ifndef BFIN_IP01
  bfin_write_DMA2_X_COUNT(samples_per_chunk*8);
#else
	switch (bfsi_widebandmode)
	{
	case 1:
		bfin_write_DMA2_X_COUNT(samples_per_chunk/2);
		  /* Inner loop address increment */
		  bfin_write_DMA2_X_MODIFY(2);
		  bfin_write_DMA2_Y_MODIFY(2);
		  bfin_write_DMA2_Y_COUNT(2);
		break;
	case 2:
		bfin_write_DMA2_X_COUNT(samples_per_chunk/2);
		  /* Inner loop address increment */
		  bfin_write_DMA2_X_MODIFY(2);
		  bfin_write_DMA2_Y_MODIFY(2);
		  bfin_write_DMA2_Y_COUNT(2);
		break;
	default:
		bfin_write_DMA2_X_COUNT(samples_per_chunk);
		  /* Inner loop address increment */
		  bfin_write_DMA2_X_MODIFY(1);
		  bfin_write_DMA2_Y_MODIFY(1);
		  bfin_write_DMA2_Y_COUNT(2);
		break;
	}
#endif


#endif

#if defined(CONFIG_BF537)

#if defined(BFSI_SPORT0)
  /* Start address of data buffer */
  bfin_write_DMA4_START_ADDR(iTxBuffer1);

  /* DMA inner loop count */
  bfin_write_DMA4_X_COUNT(samples_per_chunk*8);

  /* Inner loop address increment */
  bfin_write_DMA4_X_MODIFY(1);
  bfin_write_DMA4_Y_MODIFY(1);
  bfin_write_DMA4_Y_COUNT(2);
#endif

#if defined(BFSI_SPORT1)
  /* Start address of data buffer */
  bfin_write_DMA6_START_ADDR(iTxBuffer1);

  /* DMA inner loop count */
  bfin_write_DMA6_X_COUNT(samples_per_chunk*8);

  /* Inner loop address increment */
  bfin_write_DMA6_X_MODIFY(1);
  bfin_write_DMA6_Y_MODIFY(1);
  bfin_write_DMA6_Y_COUNT(2);
#endif

#endif

}

/* works out which write buffer is available for writing */

static u8 *isr_write_processing(void) {
        u8  *writechunk;
	int  writepingpong;
	int  x,i;

	/* select which ping-pong buffer to write to */
#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
	x = (int)(bfin_read_DMA2_CURR_ADDR()) - (int)iTxBuffer1;
#endif
#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
	x = (int)(bfin_read_DMA4_CURR_ADDR()) - (int)iTxBuffer1;
#endif
#if defined(BFSI_SPORT1)
	x = (int)(bfin_read_DMA6_CURR_ADDR()) - (int)iTxBuffer1;
#endif
#endif
	/* for some reason x for tx tends to be 0xe and 0x4e, whereas
	   x for rx is 0x40 and 0x80.  Not sure why they would be
	   different.  We could perhaps consider having
	   different interrupts for tx and rx side.  Hope this
	   offset doesn't kill the echo cancellation, e.g. if we
	   get echo samples in rx before tx has sent them!
	*/

#ifndef BFIN_IP01
	if (x >= 8*samples_per_chunk) {
		writechunk = (unsigned char*)iTxBuffer1;
		writechunk_first++;
		writepingpong = PING;
	}
	else {
		writechunk = (unsigned char*)iTxBuffer1 + samples_per_chunk*8;
		writechunk_second++;
		writepingpong = PONG;
	}
#else

	if (x > row_length) {
			writechunk = (unsigned char*)iTxBuffer1;
			writechunk_first++;
			writepingpong = PING;
		}
		else {
			writechunk = (unsigned char*)iTxBuffer1 + samples_per_chunk;
			writechunk_second++;
			writepingpong = PONG;
		}

#endif
	for(i=0; i<4; i++)
		    last_xw[i] = last_xw[i+1];
		last_xw[4] = x;
	/* make sure writechunk actually ping pongs */

	if (writepingpong == lastwritepingpong) {
		writechunk_didntswap++;
		memcpy(bad_xw, last_xw, sizeof(bad_xw));
	}
	lastwritepingpong = writepingpong;
	return writechunk;
}

/* works out which read buffer is available for reading */

static u8 *isr_read_processing(void) {
        u8 *readchunk;
	int readpingpong;
	int x,i;

	/* select which ping-pong buffer to write to */
#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
	x = (int)bfin_read_DMA1_CURR_ADDR() - (int)iRxBuffer1;
#endif
#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
	x = (int)bfin_read_DMA3_CURR_ADDR() - (int)iRxBuffer1;
#endif
#if defined(BFSI_SPORT1)
	x = (int)bfin_read_DMA5_CURR_ADDR() - (int)iRxBuffer1;
#endif
#endif
	/* possible values for x are 8*samples_per_chunk=0x40 at the
	   end of the first row and 2*8*samples_per_chunk=0x80 at the
	   end of the second row */

#ifndef BFIN_IP01
	if (x == 8*samples_per_chunk) {
		readchunk = iRxBuffer1;
		readchunk_first++;
		readpingpong = PING;
	}
	else {
		readchunk = iRxBuffer1 + samples_per_chunk*8;
		readchunk_second++;
		readpingpong = PONG;
	}
#else
	if (x > samples_per_chunk) {
			readchunk = iRxBuffer1;
			readchunk_first++;
			readpingpong = PING;
		}
		else {
			readchunk = iRxBuffer1 + samples_per_chunk;
			readchunk_second++;
			readpingpong = PONG;
		}
#endif

	log_readchunk = readchunk;

	/* memory of x for debug */

		for(i=0; i<4; i++)
		    last_xr[i] = last_xr[i+1];
		last_xr[4] = x;

	/* make sure readchunk actually ping pongs */

	if (readpingpong == lastreadpingpong) {
		readchunk_didntswap++;
		memcpy(bad_xr, last_xr, sizeof(bad_xr));
	}
	lastreadpingpong = readpingpong;

	return readchunk;
}

/* called each time the DMA finishes one "line" */

static irqreturn_t sport_rx_isr(int irq, void *dev_id, struct pt_regs * regs)
{
  unsigned int  start_cycles = cycles();
  u8           *read_samples;
  u8           *write_samples;

  /* confirm interrupt handling, write 1 to DMA_DONE bit */
#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
  bfin_write_DMA1_IRQ_STATUS(0x0001);
#endif
#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
  bfin_write_DMA3_IRQ_STATUS(0x0001);
#endif
#if defined(BFSI_SPORT1)
  bfin_write_DMA5_IRQ_STATUS(0x0001);
#endif
#endif
  __builtin_bfin_ssync();
  __builtin_bfin_ssync();

  /* Measure clock cycles since last interrupt.  This should always be less
     than 1ms.
  */

  isr_between_diff = start_cycles - isr_between_prev;
  isr_between_prev = start_cycles;
  if (isr_between_diff > isr_between_worst)
      isr_between_worst = isr_between_diff;
  /* note hard coded for 400MHz clock, 400000 cycles between ISRs, so
     800000 means we didnt process within 2ms */
  if (isr_between_diff > 800000) {
      isr_between_skip++;
      isr_between_difflastskip = isr_between_diff;
  }

  /* read and write sample callbacks */

  read_samples = isr_read_processing();
  write_samples = isr_write_processing();
  if ((bfsi_isr_callback != NULL) && !bfsi_freeze) {
    bfsi_isr_callback(read_samples, write_samples);
  }

  __builtin_bfin_ssync();

  /* some stats to help monitor the cycles used by ISR processing */

  /*
     Simple IIR averager:

       y(n) = (1 - 1/TC)*y(n) + (1/TC)*x(n)

     After conversion to fixed point:

       2*y(n) = ((TC-1)*2*y(n) + 2*x(n) + half_lsb ) >> LTC
  */

  isr_cycles_average = ( (u32)(TC-1)*isr_cycles_average +
			 (((u32)isr_cycles_last)<<1) + TC) >> LTC;

  if (isr_cycles_last > isr_cycles_worst)
    isr_cycles_worst = isr_cycles_last;

  /* we sample right at the end to make sure we count cycles used to
     measure cycles! */
  isr_cycles_last = cycles() - start_cycles;

  return IRQ_HANDLED;
}

static int init_sport_interrupts(void)
{
	//unsigned int data32;

#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
	if(request_irq(IRQ_SPORT0_RX, sport_rx_isr,
                       SA_INTERRUPT, "sport rx", NULL) != 0) {
                return -EBUSY;
        }
#endif

#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
  	if(request_irq(IRQ_SPORT0_RX, sport_rx_isr,
		       SA_INTERRUPT, "sport rx", NULL) != 0) {
    		return -EBUSY;
	}
#endif
#if defined(BFSI_SPORT1)
  	if(request_irq(IRQ_SPORT1_RX, sport_rx_isr,
		       SA_INTERRUPT, "sport rx", NULL) != 0) {
    		return -EBUSY;
	}
#endif
#endif
	if (bfsi_debug) {
		printk("ISR installed OK\n");
	}
#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
	/* enable DMA1 sport0 Rx interrupt */
	bfin_write_SIC_IMASK(bfin_read_SIC_IMASK() | 0x00000200);
	__builtin_bfin_ssync();
#endif
#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
	/* enable DMA3 sport0 Rx interrupt */
	bfin_write_SIC_IMASK(bfin_read_SIC_IMASK() | 0x00000020);
	__builtin_bfin_ssync();
#endif
#if defined(BFSI_SPORT1)
	/* enable DMA5 sport1 Rx interrupt */
	bfin_write_SIC_IMASK(bfin_read_SIC_IMASK() | 0x00000080);
	__builtin_bfin_ssync();
#endif
#endif
	return 0;
}

static void enable_dma_sport(void)
{
	/* enable DMAs */
#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
	bfin_write_DMA2_CONFIG(bfin_read_DMA2_CONFIG() | DMAEN);
	bfin_write_DMA1_CONFIG(bfin_read_DMA1_CONFIG() | DMAEN);
	__builtin_bfin_ssync();

	/* enable sport0 Tx and Rx */

	bfin_write_SPORT0_TCR1(bfin_read_SPORT0_TCR1() | TSPEN);
	bfin_write_SPORT0_RCR1(bfin_read_SPORT0_RCR1() | RSPEN);
	__builtin_bfin_ssync();
#endif

#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
	bfin_write_DMA4_CONFIG(bfin_read_DMA4_CONFIG() | DMAEN);
	bfin_write_DMA3_CONFIG(bfin_read_DMA3_CONFIG() | DMAEN);
	__builtin_bfin_ssync();

	/* enable sport0 Tx and Rx */

	bfin_write_SPORT0_TCR1(bfin_read_SPORT0_TCR1() | TSPEN);
	bfin_write_SPORT0_RCR1(bfin_read_SPORT0_RCR1() | RSPEN);
	__builtin_bfin_ssync();
#endif
#if defined(BFSI_SPORT1)
	bfin_write_DMA6_CONFIG(bfin_read_DMA6_CONFIG() | DMAEN);
	bfin_write_DMA5_CONFIG(bfin_read_DMA5_CONFIG() | DMAEN);
	__builtin_bfin_ssync();

	/* enable sport1 Tx and Rx */

	bfin_write_SPORT1_TCR1(bfin_read_SPORT1_TCR1() | TSPEN);
	bfin_write_SPORT1_RCR1(bfin_read_SPORT1_RCR1() | RSPEN);
	__builtin_bfin_ssync();
#endif
#endif

}

static void disable_sport(void)
{
#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
	/* disable sport0 Tx and Rx */

	bfin_write_SPORT0_TCR1(bfin_read_SPORT0_TCR1() & (~TSPEN));
	bfin_write_SPORT0_RCR1(bfin_read_SPORT0_RCR1() & (~RSPEN));
	__builtin_bfin_ssync();

	/* disable DMA1 and DMA2 */

	bfin_write_DMA2_CONFIG(bfin_read_DMA2_CONFIG() & (~DMAEN));
	bfin_write_DMA1_CONFIG(bfin_read_DMA1_CONFIG() & (~DMAEN));
	__builtin_bfin_ssync();
	bfin_write_SIC_IMASK(bfin_read_SIC_IMASK() & (~0x00000200));
	__builtin_bfin_ssync();
#endif

#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
	/* disable sport0 Tx and Rx */

	bfin_write_SPORT0_TCR1(bfin_read_SPORT0_TCR1() & (~TSPEN));
	bfin_write_SPORT0_RCR1(bfin_read_SPORT0_RCR1() & (~RSPEN));
	__builtin_bfin_ssync();

	/* disable DMA3 and DMA4 */

	bfin_write_DMA4_CONFIG(bfin_read_DMA4_CONFIG() & (~DMAEN));
	bfin_write_DMA3_CONFIG(bfin_read_DMA3_CONFIG() & (~DMAEN));
	__builtin_bfin_ssync();
	bfin_write_SIC_IMASK(bfin_read_SIC_IMASK() & (~0x00000020));
	__builtin_bfin_ssync();
#endif
#if defined(BFSI_SPORT1)
	/* disable sport1 Tx and Rx */

	bfin_write_SPORT1_TCR1(bfin_read_SPORT1_TCR1() & (~TSPEN));
	bfin_write_SPORT1_RCR1(bfin_read_SPORT1_RCR1() & (~RSPEN));
	__builtin_bfin_ssync();

	/* disable DMA3 and DMA4 */

	bfin_write_DMA6_CONFIG(bfin_read_DMA6_CONFIG() & (~DMAEN));
	bfin_write_DMA5_CONFIG(bfin_read_DMA5_CONFIG() & (~DMAEN));
	__builtin_bfin_ssync();
	bfin_write_SIC_IMASK(bfin_read_SIC_IMASK() & (~0x00000080));
	__builtin_bfin_ssync();
#endif
#endif
}

int bfsi_proc_read(char *buf, char **start, off_t offset,
		    int count, int *eof, void *data)
{
	int len;

	len = sprintf(buf,
		      "samples_per_chunk.......: %d\n"
			  "Wideband_mode...........: %d\n"
			  "readchunk_first.........: %d\n"
		      "readchunk_second........: %d\n"
		      "readchunk_didntswap.....: %d\n"
		      "bad_xr..................: %d %d %d %d %d\n"
		      "log_readchunk...........: 0x%08x\n"
		      "writechunk_first........: %d\n"
		      "writechunk_second.......: %d\n"
		      "writechunk_didntswap....: %d\n"
			  "bad_xw..................: %d %d %d %d %d\n"
		      "isr_cycles_last.........: %d\n"
		      "isr_cycles_worst........: %d\n"
		      "isr_cycles_average......: %d\n"
		      "echo_sams...............: %d\n"
		      "isr_between_diff........: %d\n"
		      "isr_between_worst.......: %d\n"
		      "isr_between_skip........: %d\n"
		      "isr_between_difflastskip: %d\n",
		      samples_per_chunk,
		      bfsi_widebandmode,
		      readchunk_first,
		      readchunk_second,
		      readchunk_didntswap,
		      bad_xr[0],bad_xr[1],bad_xr[2],bad_xr[3],bad_xr[4],
		      log_readchunk,
		      writechunk_first,
		      writechunk_second,
		      writechunk_didntswap,
		      bad_xw[0],bad_xw[1],bad_xw[2],bad_xw[3],bad_xw[4],
		      isr_cycles_last,
		      isr_cycles_worst,
		      isr_cycles_average>>1,
		      echo_sams,
		      isr_between_diff,
		      isr_between_worst,
		      isr_between_skip,
		      isr_between_difflastskip
);

	*eof=1;
	return len;
}

static int proc_read_bfsi_freeze(char *buf, char **start, off_t offset,
                                 int count, int *eof, void *data)
{
  int len;
  unsigned int flags;

  *eof = 1;

  len = sprintf(buf, "%d\n", bfsi_freeze);

  return len;
}

static int proc_write_bfsi_freeze(struct file *file, const char *buffer,
                                  unsigned long count, void *data)
{
  int   new_freeze;
  char *endbuffer;
  unsigned int flags;

  new_freeze = simple_strtol (buffer, &endbuffer, 10);
  bfsi_freeze = new_freeze;

  return count;
}

static int proc_write_bfsi_reset(struct file *file, const char *buffer,
                                  unsigned long count, void *data)
{
    int i;

    isr_cycles_worst = 0;
    isr_between_worst = 0;
    isr_between_skip = 0;
    isr_between_difflastskip = 0;
    readchunk_first = 0;
    readchunk_second = 0;
    readchunk_didntswap = 0;
    writechunk_first = 0;
    writechunk_second = 0;
    writechunk_didntswap = 0;
    for(i=0; i<5; i++)
    {
    	bad_xr[i] = 0;
    	bad_xw[i] = 0;
    }

    return count;
}

/*
   Wrapper for entire SPORT setup, returns 1 for success, 0 for failure.

   The SPORT code is designed to deliver small arrays of size samples
   every (125us * samples).  A ping-pong arrangement is used, so the
   address of the buffer will alternate every call between two possible
   values.

   The callback functions provide to the address of the current buffer
   for the read and write channels.  Read means the data was just
   read from the SPORT, so this is the "receive" PCM samples.  Write
   is the PCM data to be written to the SPORT.

   The callbacks are called in the context of an interrupt service
   routine, so treat any code them like an ISR.

   Once this function returns successfully the SPORT/DMA will be up
   and running, and calls to the isr callback will start.  For testing
   it is OK to set the callback function pointer to NULL, say if you
   just want to look at the debug information.

   If debug==1 then "cat /proc/bfsi" will display some debug
   information, something like:

     readchunk_first.....: 9264
     readchunk_second....: 9264
     readchunk_didntswap.: 0
     writechunk_first....: 9264
     writechunk_second...: 9264
     writechunk_didntswap: 0

   If all is well then "readchunk_didntswap" and "writechunk_didntswap"
   will be static and some very small number.  The first and second
   values should be at most one value different.  These variables
   indicate sucessful ping-pong operation.

   The numbers are incremented ever interrupt, for example if samples=8
   (typical for zaptel), then we get one interrupt every ms, or 1000
   interrupts per second.  This means the values for each first/second
   entry should go up 500 times per second.

   8 channels are sampled at once, so the size of the samples buffers
   is 8*samples (typically 64 bytes for zaptel).

   TODO:

   1/ It might be nice to modify this function allow user defined
      SPORT control reg settings, for example to change clock
      dividers and frame sync sources.  Or posible provide
      a bfsi_sport_set() function.

   2/ Modify the callbacks to provide user-dfine context information.

   3/ Modify init to define max number of channels, it is currently
      hard coded at 8.
*/

int bfsi_sport_init(
  void (*isr_callback)(u8 *read_samples, u8 *write_samples),
  int samples,
  int wideband,
  int debug
)
{
  struct proc_dir_entry *freeze, *reset;

  if (debug) {
    create_proc_read_entry("bfsi", 0, NULL, bfsi_proc_read, NULL);
    bfsi_debug = debug;

    freeze = create_proc_read_entry("bfsi_freeze", 0, NULL, proc_read_bfsi_freeze, NULL);
    freeze->write_proc = proc_write_bfsi_freeze;
    reset = create_proc_read_entry("bfsi_reset", 0, NULL, NULL, NULL);
    reset->write_proc = proc_write_bfsi_reset;
  }

  bfsi_isr_callback = isr_callback;
  bfsi_widebandmode = wideband;
	switch (bfsi_widebandmode)
	{
	case 1:
		samples_per_chunk = samples * 4;
		row_length = 28;
		break;
	case 2:
		samples_per_chunk = samples * 2;
		row_length = 12;
		break;
	default:
		samples_per_chunk = samples;
		row_length = samples_per_chunk;
		break;
	}

#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
  init_sport0();
#endif
#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
  init_sport0();
#endif
#if defined(BFSI_SPORT1)
  init_sport1();
#endif
#endif
  init_dma_wc();
  enable_dma_sport();

  if (init_sport_interrupts())
    init_ok = 0;
  else
    init_ok = 1;

  return init_ok;
}

/* shut down SPORT operation cleanly */

void bfsi_sport_close(void)
{
  disable_sport();

  if (init_ok) {
#if (defined(CONFIG_BF533) || defined(CONFIG_BF532))
    free_irq(IRQ_SPORT0_RX, NULL);
#endif
#if defined(CONFIG_BF537)
#if defined(BFSI_SPORT0)
    free_irq(IRQ_SPORT0_RX, NULL);
#endif
#if defined(BFSI_SPORT1)
    free_irq(IRQ_SPORT1_RX, NULL);
#endif
#endif
  }
#if L1_DATA_A_LENGTH != 0
  l1_data_A_sram_free(iTxBuffer1);
  l1_data_A_sram_free(iRxBuffer1);
#else
  dma_free_coherent(NULL, 2*samples_per_chunk*8, iTxBuffer1, 0);
  dma_free_coherent(NULL, 2*samples_per_chunk*8, iRxBuffer1, 0);
#endif
  remove_proc_entry("bfsi", NULL);
  remove_proc_entry("bfsi_freeze", NULL);
  remove_proc_entry("bfsi_reset", NULL);
}

MODULE_LICENSE("GPL");
EXPORT_SYMBOL(bfsi_spi_write_8_bits);
EXPORT_SYMBOL(bfsi_spi_read_8_bits);
EXPORT_SYMBOL(bfsi_spi_init);
EXPORT_SYMBOL(bfsi_sport_init);
EXPORT_SYMBOL(bfsi_reset);
EXPORT_SYMBOL(bfsi_sport_close);

