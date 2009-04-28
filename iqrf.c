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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "liqrf.h"

#define debug_iqrf(format, arg...) \
	if (verbose) {\
		printf("iqrf:" format , ## arg); \
	} else {}\

/* send data in object via usb */
int usb_send_data(struct liqrf_obj *obj) 
{
	int ret_val = 0;
	if  (send_receive_packet(obj->dev_handle, (char *)obj->tx_buff, obj->tx_len,
						(char *)obj->rx_buff, obj->rx_len)) 
		ret_val = 1;
	
	return ret_val;
}

/* send data in object via usb */
int usb_write_data(struct liqrf_obj *obj) 
{
	int ret_val = 0;
	if  (send_packet(obj->dev_handle, (char *)obj->tx_buff, obj->tx_len))
		ret_val = 1;
	
	return ret_val;
}

/* fill transmission buffer with enter prog mode data */
void enter_prog_mode(struct liqrf_obj *obj)
{
	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));
	memset(obj->rx_buff, 0, sizeof(obj->rx_buff));

	obj->tx_buff[0] = CMD_ENTER_PROG;
	obj->tx_buff[1] = UNKNOWN_PROG;
	obj->tx_buff[2] = count_crc_tx(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);
	
	obj->tx_len = 3;
	obj->rx_len = 3;
	
	usb_write_data(obj);
}


/* fill transmission buffer with enter prog mode data 1
   which are prepare for retrieve module id
   data: 01 F5 81 00 2B
   rece: 81 81 00 DE 3F 3F 48 48 48 48 
*/
int request_module_id(struct liqrf_obj *obj)
{
	int ret_val = 0;

	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));
	memset(obj->rx_buff, 0, sizeof(obj->rx_buff));
	
	obj->tx_buff[0] = CMD_FOR_CK;
	obj->tx_buff[1] = 0xF5;
	obj->tx_buff[2] = 0x81;
	obj->tx_buff[3] = 0x00;
	obj->tx_buff[4] = count_crc_tx(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);
	
	obj->tx_len = 11;
	/* hardcoded from log */
	obj->rx_len = 10;
	
	usb_send_data(obj);
	if (!check_crc_rx(&obj->rx_buff[2], 0x81, obj->rx_len - 3))
		ret_val = 1;

	return ret_val;
	
}


/* fill transmission buffer with enter prog mode data 2
   which are data with module id
   example:
   data: 01 F0 08 00 00 00 00 00 00 00 00 A7 00 00
   rece: 48 48 01 00 10 4F 22 02 30 03 1A 3F 81
*/
int get_module_id(struct liqrf_obj *obj)
{
	int ret_val = 0, i = 0;
	unsigned char spi_stat = SPI_DISABLED;

	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));
	memset(obj->rx_buff, 0, sizeof(obj->rx_buff));
	
	obj->tx_buff[0] = CMD_FOR_CK;
	obj->tx_buff[1] = 0xF0;
	obj->tx_buff[2] = 0x08;
	
	obj->tx_buff[11] = count_crc_tx(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);
	
	obj->tx_len = 14;
	obj->rx_len = 13;
	
	while (spi_check_data(spi_stat) != DATA_READY) {
		usb_send_data(obj);
		spi_stat = obj->rx_buff[1];
		usleep(100000);	
	}
	
	if (!check_crc_rx(&obj->rx_buff[2], 0x08, obj->rx_len - 5))
		ret_val = 1;
	
	printf("Module ID:");
	for (i = 0; i < 4; i++)
		printf("%02X ", obj->rx_buff[2+i]);
	printf("\n");

	return ret_val;
}


/* fill transmission buffer with spi check command */
int enter_endprog_mode(struct liqrf_obj *obj)
{
	int ret_val = 0;

	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));
	memset(obj->rx_buff, 0, sizeof(obj->rx_buff));
	
	obj->tx_buff[0] = CMD_FOR_CK;
	obj->tx_buff[1] = 0xF3;
	obj->tx_buff[2] = 0x83;
	obj->tx_buff[3] = 0xDE;
	obj->tx_buff[4] = 0x01;
	obj->tx_buff[5] = 0xFF;
	obj->tx_buff[6] = count_crc_tx(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);
	
	obj->tx_len = 9;
	obj->rx_len = 8;

	usb_send_data(obj);

	if (!check_crc_rx(&obj->rx_buff[2], 0x83, obj->rx_len - 3))
		ret_val = 1;
	return ret_val;
}

/* first check spi status */
int check_prog_mode(struct liqrf_obj *obj)
{
	unsigned char spi_stat = SPI_DISABLED;
	int ret_val = 0;

	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));
	memset(obj->rx_buff, 0, sizeof(obj->rx_buff));
	/* prepare data for check spi status */	
	obj->tx_buff[0] = CMD_FOR_CK;
	obj->tx_buff[1] = 0;
	obj->tx_buff[2] = 0;
	obj->tx_buff[3] = 0;
	obj->tx_buff[4] = 0;
	
	obj->tx_len = 5;
	obj->rx_len = 4;
	
	while (spi_stat != PROGRAMMING_MODE) {
		if (usb_send_data(obj)) 
			return ret_val;
		
		spi_stat = obj->rx_buff[1];
		debug_iqrf("spistat = %X\n", spi_stat);
		usleep(100000);
	}
	ret_val = 1;
	return ret_val;
}



/* prepare data for usb transmission */
void prepare_prog_data(int data_type, unsigned char *data, int data_len, 
			unsigned short addr, struct liqrf_obj *obj)
{
	int i, data_fill = 0;

	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));

	switch (data_type) {
	case EEPROM_USER:
		obj->tx_buff[0] = CMD_PROG;
		obj->tx_buff[1] = EEPROM_DATA;
		obj->tx_buff[2] = 0x83;
		/* address is just one byte */
		obj->tx_buff[3] = addr & 0xFF;
		obj->tx_buff[4] = data_len;

		memcpy(&obj->tx_buff[5], data, data_len);
		obj->tx_buff[5 + data_len] = count_crc_tx(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);
		debug_iqrf("usr eeprom data len = %d\n", data_len);
	
		obj->tx_len = BUF_LEN;
		obj->rx_len = BUF_LEN;	
		break;
	case EEPROM_APP:
		obj->tx_buff[0] = CMD_PROG;
		obj->tx_buff[1] = EEPROM_DATA;
		obj->tx_buff[2] = UNKNOWN;
		/* address is just one byte */
		obj->tx_buff[3] = addr & 0xFF;
		obj->tx_buff[4] = data_len;

		memcpy(&obj->tx_buff[5], data, data_len);
		obj->tx_buff[5 + data_len] = count_crc_tx(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);
		debug_iqrf("app eeprom data len = %d\n", data_len);
	
		obj->tx_len = BUF_LEN;
		obj->rx_len = BUF_LEN;			
		break;
	
	case FLASH_PROG:
		obj->tx_buff[0] = CMD_PROG;
		obj->tx_buff[1] = FLASH_DATA;
		obj->tx_buff[2] = UNKNOWN;
		obj->tx_buff[3] = addr & 0xFF;
		obj->tx_buff[4] = (addr >> 8) & 0xFF;

		memcpy(&obj->tx_buff[5], data, data_len);
		/* if we have less data then block size 
		   fill it with 0xFF and 0x3F
		*/
		if (data_len < FLASH_BLOCK_SIZE) {
			data_fill = FLASH_BLOCK_SIZE - data_len;
			
			for (i = 0; i < data_fill; i++) {
				obj->tx_buff[5 + data_len + i++] = 0xFF;
				obj->tx_buff[5 + data_len + i] = 0x3F;
			}
		} 
		obj->tx_buff[5 + data_len + data_fill] = count_crc_tx(&obj->tx_buff[1], 
			sizeof(obj->tx_buff) -1);

		debug_iqrf("flash data len = %d\n", data_len);
			
		obj->tx_len = BUF_LEN;
		obj->rx_len = BUF_LEN;
	
		break;
	default:
		fprintf(stderr, "Unsupported data!\n");
		break;
	}
}

