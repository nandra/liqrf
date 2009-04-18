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

/* count checksum */
unsigned char count_checksum(unsigned char *data, int len)
{
	int i = 0; 
	/* value defined in SPI docu */
	int result = 0x5f;

	for (i = 0; i < len; i++)
		result ^= data[i];
	
	return result;
}

/* send data in object via usb */
int usb_send_data(struct liqrf_obj *obj) 
{
	int ret_val = 0;
	if  (send_receive_packet(obj->dev_handle, (char *)obj->tx_buff, obj->tx_len,
						(char *)obj->rx_buff, obj->rx_len))
		ret_val = 1;
	
	return ret_val;
}

/* fill transmission buffer with enter prog mode data */
void enter_prog_mode(struct liqrf_obj *obj)
{
	int i = 0;

	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));
	memset(obj->rx_buff, 0, sizeof(obj->rx_buff));

	obj->tx_buff[0] = CMD_ENTER_PROG;
	obj->tx_buff[1] = UNKNOWN_PROG;
	obj->tx_buff[2] = count_checksum(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);
	
	obj->tx_len = 3;
	obj->rx_len = 3;
	for (i = 0; i < BUF_LEN; i++)
			printf("%02x ", obj->tx_buff[i]);		

	printf("\n");	
	usb_send_data(obj);
}

/* fill transmission buffer with spi check command */
void prepare_spi_check_mode(struct liqrf_obj *obj)
{
	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));
	memset(obj->rx_buff, 0, sizeof(obj->rx_buff));
	
	obj->tx_buff[0] = CMD_FOR_CK;
	obj->tx_buff[1] = 0;
	obj->tx_buff[2] = 0;
	obj->tx_buff[3] = 0;
	obj->tx_buff[4] = 0;
	
	obj->tx_len = 5;
	obj->rx_len = 4;
}


/* fill transmission buffer with spi check command */
void enter_endprog_mode(struct liqrf_obj *obj)
{
	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));
	memset(obj->rx_buff, 0, sizeof(obj->rx_buff));
	
	obj->tx_buff[0] = CMD_FOR_CK;
	obj->tx_buff[1] = 0xF3;
	obj->tx_buff[2] = 0x83;
	obj->tx_buff[3] = 0xDE;
	obj->tx_buff[4] = 0x01;
	obj->tx_buff[5] = 0xFF;
	obj->tx_buff[6] = count_checksum(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);;
	
	obj->tx_len = 7;
	obj->rx_len = 8;
	usb_send_data(obj);
}

/* first check spi status */
int check_prog_mode(struct liqrf_obj *obj)
{
	unsigned char spi_stat = SPI_DISABLED;
	int ret_val = 0;
	
	prepare_spi_check_mode(obj);

	while (spi_stat != PROGRAMMING_MODE) {
		if (usb_send_data(obj)) 
			return ret_val;
		
		spi_stat = obj->rx_buff[1];
		printf("%s spistat = %X", __FUNCTION__, spi_stat);
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
	case EEPROM_PROG:
		obj->tx_buff[0] = CMD_PROG;
		obj->tx_buff[1] = EEPROM_DATA;
		obj->tx_buff[2] = UNKNOWN;
		/* address is just one byte */
		obj->tx_buff[3] = addr & 0xFF;
		/*FIXME this is currently not known just suppose */
		obj->tx_buff[4] = data_len;

		memcpy(&obj->tx_buff[5], data, data_len);
		obj->tx_buff[5 + data_len] = count_checksum(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);
		printf("eeprom data len = %d\n", data_len);
		for (i = 0; i < BUF_LEN; i++)
			printf("%02x ", obj->tx_buff[i]);		

		printf("\n");				
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
		obj->tx_buff[5 + data_len + data_fill] = count_checksum(&obj->tx_buff[1], 
			sizeof(obj->tx_buff) -1);

		printf("flash data len = %d\n", data_len);
		for (i = 0; i < BUF_LEN; i++)
			printf("%02x ", obj->tx_buff[i]);		

		printf("\n");		
		break;
	default:
		fprintf(stderr, "Unsupported data!\n");
		break;
	}
}

