/*
fx.c
May 10, 2008

Based on:	fx.c    by David Rowe 
Device driver supporting one fxs SLIC  on the
Blackfin

TODO: 
*/

/*
Copyright (C) 2008 ATCom

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
#include <asm/io.h>
#include <asm/bfin5xx_spi.h>
#include <linux/spi/spi.h>


#define LED_OFF        0
#define LED_ON	       1
#define PF_FXS_LED	   6
#define PF_SYS_LED     7

#define FX_LED_RED     1
#define FX_LED_GREEN   1

/* Keep these obsolete macros */
#ifndef BFIN_IP01
#define FX_MAX_PORTS   8  // max number of ports in system
#else
#define FX_MAX_PORTS   1
#endif

static u16 flag_enable, flag;

static inline u16 read_FLAG(void)
{
	return *(volatile unsigned short*)(SPI0_REGBASE + 0x04);
}

static inline void write_FLAG(u16 v)
{
	*(volatile unsigned short*)(SPI0_REGBASE + 0x04) = v;
	__builtin_bfin_ssync();
}


inline void fx_set_led(int port, int state)
{ 
	//flag = read_FLAG();
	//flag_enable = flag & ~(1 << (PF_FXS_LED + 8));
	/* PF_FXS_LED is less than 8, so we use write_FLAG function */
	if ( LED_ON ==  state )
	{
		/* Clear PFx used for chip_select */
		bfin_write_FIO_FLAG_S(1<<PF_FXS_LED); 
	}
	else
	{
		/* Raise High of PFx */
		bfin_write_FIO_FLAG_C(1<<PF_FXS_LED);
	}
}

// read register 0x2 of 3050 FXO
char fx_read_fxo(int port) {
	u8  reg;

	//bfsi_spi_write_8_bits(SPI_NCSB, port); 
	bfsi_spi_write_8_bits(SPI_NCSA, 0x60);
	bfsi_spi_write_8_bits(SPI_NCSA, 0x02);
	reg =  bfsi_spi_read_8_bits(SPI_NCSA);

	return reg;
}

// read register 0x0 of 3210 FXS
/* According to the 3215 datasheet, we need to read the direct register 0x1, the Bit7 is identification of part number
when 0 is Si3210 family or 1 is Si3215 family  */
char fx_read_fxs(int port, u8 bits) {
	u8  reg;

	if (port > 4)
		port += 0x40 - 4;

	//bfsi_spi_write_8_bits(SPI_NCSB, port); 
	bfsi_spi_write_8_bits(SPI_NCSA, bits | 0x80);
	reg =  bfsi_spi_read_8_bits(SPI_NCSA);

	return reg;
}

void fx_auto_detect(char port_type[], int bit_reset) {
	int i;
	u8  reg;
	char fxdebug = 0;

       // Add LED initialization
       bfin_write_FIO_DIR(bfin_read_FIO_DIR() | (1<<PF_FXS_LED) | (1<<PF_SYS_LED) ); 

       bfin_write_FIO_FLAG_S(1<<PF_SYS_LED); 
       
#if 1
       bfsi_reset(bit_reset);
	for(i=0; i<FX_MAX_PORTS; i++) {
		port_type[i] = '-';

		bfsi_reset(bit_reset);
		reg = fx_read_fxo(i+1);
		if (reg == 0x3) {
			port_type[i] = 'O';
			fx_set_led(i+1, FX_LED_RED);
		}
		else {
			bfsi_reset(bit_reset);
			reg = fx_read_fxs(i+1, 0);
			
			if (reg == 0x5 ) {
				port_type[i] = 'S';
				fx_set_led(i+1, FX_LED_GREEN);
			}
			else
			{
				/* Check whether Si3215 */
				bfsi_reset(bit_reset);
				reg = fx_read_fxs(i+1, 0x1);
				
				/* As mentioned in datasheet of Si3215, the register 0x1 
				Bit7 is identification 2, and Bit6 is Reserved which is read 
				as zero */
				if ( ( reg & 0xC0) == 0x80 )
				{
					port_type[i] = 'S';
					fx_set_led(i+1, FX_LED_GREEN);
				}
			}
		}
	}
#endif
}

