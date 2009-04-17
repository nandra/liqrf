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

void print_help(void)
{
	fprintf(stderr, "Using ./liqrf -f <hex file> [-v]\n");		
	fprintf(stderr, "-f <hex file> - hex file for loading \n");
	fprintf(stderr, "-v - verbose output\n");
}

unsigned char count_checksum(unsigned char *data, int len)
{
	int i = 0, result = 0;

	for (i = 0; i < len; i++)
		result ^= data[i];
	
	return result;
}

/* prepare data for usb transmission */
int prepare_usb_data(int data_type, unsigned char *data, int data_len, 
			unsigned short addr, struct liqrf_obj *obj)
{
	int i, data_fill = 0;

	memset(obj->tx_buff, 0, sizeof(obj->tx_buff));

	switch (data_type) {
	case EEPROM_PROG:
		obj->tx_buff[0] = PROG_MODE;
		obj->tx_buff[1] = EEPROM_DATA;
		obj->tx_buff[2] = UNKNOWN;
		/* address is just one byte */
		obj->tx_buff[3] = addr & 0xFF;
		/*FIXME this is currently not known suppose */
		obj->tx_buff[4] = data_len;

		memcpy(&obj->tx_buff[5], data, data_len);
		obj->tx_buff[5 + data_len] = 0x5f ^ count_checksum(&obj->tx_buff[1], sizeof(obj->tx_buff) -1);
		printf("eeprom data len = %d\n", data_len);
		for (i = 0; i < BUF_LEN; i++)
			printf("%02x ", obj->tx_buff[i]);		

		printf("\n");				
		break;
	
	case FLASH_PROG:
		obj->tx_buff[0] = PROG_MODE;
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
		obj->tx_buff[5 + data_len + data_fill] = 0x5f ^ count_checksum(&obj->tx_buff[1], 
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
	return 0;
}

int main (int argc, char **argv)
{
	int verbose = 0;
	struct liqrf_obj liqrf;
	char *hex_file = NULL;
	int c;
	program_data *hex_data;
	opterr = 0;
	int flash_addr = FLASH_BASE_ADDR;
	int len = 0, block_count;

	if (argc < 2) {
		fprintf (stderr, "You need to specify input hex file\n\n");
		print_help();
		goto exit;
	}
	
	while ((c = getopt (argc, argv, "hvf:")) != -1) {
  		switch (c) {
		case 'v':
 			verbose = 1;
 			break;
    		case 'f':
      			hex_file = optarg;
      			break;
		case 'h':
			print_help();			
			break;
    		case '?':
      			if (optopt == 'f') {
        			fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			} else if (isprint (optopt)) {
        			fprintf (stderr, "Unknown option `-%c'.\n\n", optopt);
				print_help();
			} else {
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			}
      			goto exit;
			break;
    		default:
      			goto exit;
			break;
    		}
	}

// 	liqrf.dev = liqrf_device_init();
// 	
// 	if (liqrf.dev == NULL) {
// 		fprintf(stderr, "Could not init device\n");
// 		goto exit;
// 	}
// 
// /* FIXME: use global debug or verbose level */
// // 	usb_set_debug(dbg_level);
// 
// 	liqrf.dev_handle = liqrf_device_open(liqrf.dev);
// 
// 	if (liqrf.dev_handle == NULL) {
// 		fprintf(stderr, "Could not open device\n");
// 		goto exit;
// 	}
	hex_data = hex_get_data(hex_file);

	if (hex_data == NULL)
		goto exit;

	/* first start prepare eeprom data */
	if (hex_data->eeprom_size) 
		prepare_usb_data(EEPROM_PROG, hex_data->eeprom, hex_data->eeprom_size, 
				EEPROM_BASE_ADDR, &liqrf);
	
	/* prepare flash data by 32 bytes blocks */
	block_count = hex_data->flash_size / FLASH_BLOCK_SIZE;
	
	if (hex_data->flash_size % FLASH_BLOCK_SIZE) 
		block_count++;
	
	for (c = 0; c < block_count; c++) {
		if ((hex_data->flash_size - (c * FLASH_BLOCK_SIZE)) < FLASH_BLOCK_SIZE)
			len = hex_data->flash_size - (c * FLASH_BLOCK_SIZE);
		else 
			len = FLASH_BLOCK_SIZE;

		prepare_usb_data(FLASH_PROG, &hex_data->flash[c*FLASH_BLOCK_SIZE], len, 
			flash_addr, &liqrf);	
		flash_addr += FLASH_ADDR_STEP;
	}	
	
exit:	
	if (hex_data)
		hex_free_memory(hex_data);
	return 0;
}
