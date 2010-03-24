/*
 * Header File:	sport_interface.h
 *
 * Based on:	bfsi.c    by David Rowe 
 *                  bfin_sport.c   by Roy Huang (roy.huang@analog.com)
 *
 * Author:	Alex Tao <wosttq@gmail.com>
 * 
 * Created:	Jun 3, 2007
 * Description: This device driver enables SPORT1 on Blackfin532 interfacing to Silicon Labs chips.
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
#ifndef __SPORT_INTERFACE_H
#define __SPORT_INTERFACE_H


/*---------------- SPORT interface function -------------------*/
extern void sport_tx_byte(u16 chip_select, u8 bits);

extern u8 sport_rx_byte(u16 chip_select);

extern void sport_interface_cleanup(void);

extern int sport_interface_init(int baud, u16 new_chip_select_mask);

extern void sport_interface_reset(int reset_bit);

#endif

