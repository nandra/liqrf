/***************************************************************************
 *   Copyright (C) 2009 by open-nandra                                     *
 *   matej.haulik@gmail.com                                                *
 *   marek.belisko@open-nandra.com                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef LIQRF_H
#define LIQRF_H

#include "usb.h"
#include "spi.h"
#include "hex_parser.h"

#define DEBUG

#define BUF_LEN 64

/* object for holding global variables */
struct liqrf_obj {
	struct usb_device *dev;
	struct usb_dev_handle *dev_handle;
	unsigned char rx_buff[BUF_LEN];
	unsigned char tx_buff[BUF_LEN];
	int tx_len;
	int rx_len;
};

#define EEPROM_APP 0x01
#define EEPROM_USER 0x02
#define FLASH_PROG 0x03

#define FLASH_BLOCK_SIZE (32) 
#define APP_EEPROM_BASE_ADDR (0xA0)
#define USR_EEPROM_BASE_ADDR (0x00)
#define FLASH_BASE_ADDR (0x0E00)
#define FLASH_ADDR_STEP (0x10)

unsigned char count_checksum(unsigned char *data, int len);
void enter_prog_mode(struct liqrf_obj *obj);
int request_module_id(struct liqrf_obj *obj);
int get_module_id(struct liqrf_obj *obj);	
int enter_endprog_mode(struct liqrf_obj *obj);
int check_prog_mode(struct liqrf_obj *obj);
void prepare_prog_data(int data_type, unsigned char *data, int data_len, 
			unsigned short addr, struct liqrf_obj *obj);
int usb_send_data(struct liqrf_obj *obj);
int usb_write_data(struct liqrf_obj *obj);

extern int verbose;
#endif