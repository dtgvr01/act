/*
 * File:	sport_interface.c
 *
 * Author:	Alex Tao <wosttq@gmail.com>
 *
 * Based on:	bfsi.c    by David Rowe 
 *                  bfin_sport.c   by Roy Huang (roy.huang@analog.com)
 * 
 * Created:	Jun 3, 2007
 * Description: This device driver enables SPORT1 on Blackfin532 interfacing 
 *              to Silicon Labs chips.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <asm/blackfin.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#include "sport_interface.h"


//#define SPORT_INTERFACE_DEBUG
#ifdef SPORT_INTERFACE_DEBUG
#define PRINTK(args...) printk(args)
#else
#define PRINTK(args...)
#endif

#ifdef SPORT_INTERFACE_DEBUG
static int txCnt = 0, rxCnt = 0;
#endif

#ifndef SPI_NCSB
/* Use other PF signals */
#define SPI_NCSA    8    /* Simulate SPORT interface as SPI */
#define SPI_NCSB    9
#endif
#define SPORT_nPWR	12

#ifndef SPORT1_REGBASE
#define SPORT1_REGBASE 0xFFC00900
#endif

#define DEFINE_SPORT1_REG(reg, off) \
static inline u16 sport1_read_##reg(void) \
            { return *(volatile unsigned short*)(SPORT1_REGBASE + off); } \
static inline void sport1_write_##reg(u16 v) \
            {*(volatile unsigned short*)(SPORT1_REGBASE + off) = v;\
             __builtin_bfin_ssync();}


DEFINE_SPORT1_REG(TCR1,0x00)
DEFINE_SPORT1_REG(TCR2,0x04)
DEFINE_SPORT1_REG(TCLKDIV,0x08)
DEFINE_SPORT1_REG(TFSDIV,0x0C)

DEFINE_SPORT1_REG(RCR1, 0x20)
DEFINE_SPORT1_REG(RCR2, 0x24)
DEFINE_SPORT1_REG(RCLKDIV,0x28)
DEFINE_SPORT1_REG(RFSDIV,0x2C)
DEFINE_SPORT1_REG(STAT,0x30)


static int sport_configure(int baud);


/* When FX_MODE, we need not fill the struct of sport_config */
static int sport_configure(int baud)
{
	/* Register SPORTx_TCR1 ( relative details pls refer 12-12 of hardware reference ) 
	        TCKFE ( Clock Falling Edge Select )   ( Bit14 ) 
          	LTFS ( Bit11) :  0 - Active high TFS; 1 - Active low TFS
		ITFS ( Bit9 ) :  0 - External TFS used; 1 - Internal TFS used 
		TFSR: 0 - Dose not require TFS for every data word;  1 - Requires TFS for every data word
		TLSBIT: 0 - Transmit MSB first ;  1 - Transmit LSB first
		ITCLK: 0 - External transmit clock selected; 1 - Internal transmit clock selected
	*/
	sport1_write_TCR1(TCKFE | LATFS | LTFS | TFSR | ITFS | ITCLK);
	
	/* 8 bit word length  */
	sport1_write_TCR2(0x7);
    
	/* SPORTx_TCLK frequency = ( System Clock frequency ) / ( 2 * ( SPORTx_TCLKDIV + 1 ) )   */
	sport1_write_TCLKDIV(baud);
	sport1_write_TFSDIV(0x7);
	
	/* Initialization of SPORT1_RCR1 and SPORT1_RCR2, similar to the Register SPORTx_TCR1  and SPORTx_TCR2 */
	sport1_write_RCR1(RCKFE | LARFS | LRFS | RFSR);  
	sport1_write_RCR2(0x7);      /* 8 bit word length      */
	
	sport1_write_RCLKDIV(baud);
	sport1_write_RFSDIV(0x7);
	
	__builtin_bfin_ssync();

	PRINTK("tcr1:0x%x, tcr2:0x%x, rcr1:0x%x, rcr2:0x%x\n",
		sport1_read_TCR1(), sport1_read_TCR2(),
		sport1_read_RCR1(), sport1_read_RCR2());

	return 0;
}


void sport_tx_byte(u16 chip_select, u8 bits)
{
	u16 tmp;

	/* drop chip select */
	if ( chip_select >7 )
	{
		/* Clear PFx used for chip_select */
		bfin_write_FIO_FLAG_C( 1<<chip_select ); 
		__builtin_bfin_ssync();
		ndelay(25);	//tsu1 - Setup Time, /CS to SCLK fall
	}
	
	tmp = bits & 0x0ff;
	bfin_write_SPORT1_TX16(tmp);

	while (!(sport1_read_STAT() & TXHRE));

	/*  Wait for the last byte sent out  */
	udelay(2);

	/* Raise chip select */
	if ( chip_select >7 )
	{
		/* Raise PFx High*/
		bfin_write_FIO_FLAG_S((1<<chip_select)); 
		__builtin_bfin_ssync();
	}

#ifdef SPORT_INTERFACE_DEBUG
	txCnt++;
	PRINTK("Send the %d byte OK!\n", txCnt);
#endif
	return;
}

u8 sport_rx_byte(u16 chip_select)
{
	u8 ret = 0;
	PRINTK("Come into %s\n",__FUNCTION__);

	/* Enable the receive operation */
	sport1_write_RCR1( sport1_read_RCR1() | RSPEN ); 
	while (!(sport1_read_RCR1() & RSPEN));
	
	/* drop chip select */
	if ( chip_select >7 )
	{
		/* Clear PFx used for chip_select */
		bfin_write_FIO_FLAG_C( 1<<chip_select ); 
		__builtin_bfin_ssync();
	}

	/* Write a dummy byte to generate a FSYNC */
	bfin_write_SPORT1_TX16(0x00ff);

	while (!(sport1_read_STAT() & RXNE))
	{
		PRINTK("%s Line%d:status:%x  %d \n", __FUNCTION__, __LINE__, sport1_read_STAT(), txCnt);
	}
	ret = (u8)bfin_read_SPORT1_RX16();
	PRINTK("%s Line%d: tcr1:0x%x, tcr2:0x%x, rcr1:0x%x, rcr2:0x%x\n",
		__FUNCTION__, __LINE__,
		sport1_read_TCR1(), sport1_read_TCR2(),
		sport1_read_RCR1(), sport1_read_RCR2());

	/* Raise chip select */
	if ( chip_select >7 )
	{
		/* Raise High of PFx */
		bfin_write_FIO_FLAG_S((1<<chip_select)); 
		__builtin_bfin_ssync();
	}

	sport1_write_RCR1( sport1_read_RCR1() & ~(RSPEN) ); 
	ndelay(200);
	__builtin_bfin_ssync();

	PRINTK("%s Line%d receive byte OK!\n",__FUNCTION__, __LINE__ );

	return ret;
}


 void sport_interface_cleanup(void)
{
	sport1_write_TCR1( sport1_read_TCR1() & ~(TSPEN) ); 
	sport1_write_RCR1( sport1_read_RCR1() & ~(RSPEN) ); 
	__builtin_bfin_ssync();
}


int sport_interface_init(int baud, u16 new_chip_select_mask)
{
	sport_configure(baud); /* Default should be 0x1 */

	PRINTK("Before setting, FIOD_DIR = 0x%04x\n", bfin_read_FIO_DIR());
	PRINTK("Before setting, new_chip_select_mask = 0x%04x\n", new_chip_select_mask);
	if (new_chip_select_mask & 0xff00) 
	{
		bfin_write_FIO_DIR(bfin_read_FIO_DIR() | 
				   (new_chip_select_mask & 0xff00) |
				   (1<<SPORT_nPWR)
				   ); 
		__builtin_bfin_ssync();
	}
	PRINTK("After setting, FIOD_DIR = 0x%04x\n", bfin_read_FIO_DIR());

	bfin_write_FIO_FLAG_S( 1 << SPORT_nPWR );
	
	//Enable SPORT1 interface
	sport1_write_TCR1( sport1_read_TCR1() | TSPEN );

	return 0; /* succeed */
}

void sport_interface_reset(int reset_bit)
{
	/* Port from David! Thanks, David!*/
  	PRINTK("set reset to PF%d\n",reset_bit);
  	bfin_write_FIO_DIR(bfin_read_FIO_DIR() | (1<<reset_bit)); 
  	__builtin_bfin_ssync();

  	bfin_write_FIO_FLAG_C((1<<reset_bit)); 
  	__builtin_bfin_ssync();
  	udelay(100);

  	bfin_write_FIO_FLAG_S((1<<reset_bit));
  	__builtin_bfin_ssync();
	
  	/*
  	p24 3050 data sheet, allow 1ms for PLL lock, with
  	less than 1ms (1000us) register 2 would have
  	a value of 0 rather than 3, indicating a bad reset.
  	*/
  	udelay(1000); 
}

EXPORT_SYMBOL( sport_tx_byte );
EXPORT_SYMBOL( sport_rx_byte );
EXPORT_SYMBOL( sport_interface_init );
EXPORT_SYMBOL( sport_interface_reset );
EXPORT_SYMBOL( sport_interface_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alex Tao <wosttq@gmail.com>");


