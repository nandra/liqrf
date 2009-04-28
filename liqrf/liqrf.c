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

/* global verbose level */
int verbose = 0;


/* usage help printing */
void print_help(void)
{
	fprintf(stderr, "Using ./liqrf -f <hex file> [-v]\n");		
	fprintf(stderr, "-f <hex file> - hex file for loading \n");
	fprintf(stderr, "-v - verbose output\n");
}


int main (int argc, char **argv)
{
	struct liqrf_obj liqrf;
	char *hex_file = NULL;
	program_data *hex_data = NULL;
	opterr = 0;
	int flash_addr = FLASH_BASE_ADDR;
	int len = 0, block_count, c;

	if (argc < 2) {
		fprintf (stderr, "You need to specify input hex file name!\n\n");
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
			goto exit;			
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
	
	liqrf.dev = iqrf_device_init();
	
	if (liqrf.dev == NULL) {
		fprintf(stderr, "Could not init device\n");
		goto exit;
	}
	
	if (verbose)
		usb_set_debug(verbose + 2);

	liqrf.dev_handle = iqrf_device_open(liqrf.dev);

	if (liqrf.dev_handle == NULL) {
		fprintf(stderr, "Could not open device\n");
		goto exit;
	}
	/* get eeprom and flash data */
	hex_data = hex_get_data(hex_file);

	if (hex_data == NULL)
		goto exit;

	/* enter to prog mode */	
	enter_prog_mode(&liqrf);
	
	/* check if module is in prog. mode */
	if (!check_prog_mode(&liqrf))
		goto exit;

	/* ask iqrf for module ID, os version and
	   mcu type
	*/
	if (request_module_id(&liqrf))
		goto exit;

	if (get_module_id(&liqrf))
		goto exit;

	/* send spi status cmd and wait for programming status */
	if (check_prog_mode(&liqrf)) {
		fprintf(stderr, "Eeprom length = %d\n", hex_data->usr_eeprom_size + 
			hex_data->app_eeprom_size);
		fprintf(stderr, "Flash instructions = %d\n", hex_data->flash_size / 2);
		/* user eeprom data */
		if (hex_data->usr_eeprom_size)
			prepare_prog_data(EEPROM_USER, hex_data->usr_eeprom, hex_data->usr_eeprom_size, 
					USR_EEPROM_BASE_ADDR, &liqrf);
		fprintf(stderr, "Programming user eeprom (%d)\n", hex_data->usr_eeprom_size);	
		usb_send_data(&liqrf);
		
		/* application eeprom data */
		if (hex_data->app_eeprom_size) 
			prepare_prog_data(EEPROM_APP, hex_data->app_eeprom, hex_data->app_eeprom_size, 
					APP_EEPROM_BASE_ADDR, &liqrf);
		fprintf(stderr, "Programming application eeprom (%d)\n", hex_data->app_eeprom_size);
		usb_send_data(&liqrf);
	
		/* prepare flash data by 32 bytes blocks */
		block_count = hex_data->flash_size / FLASH_BLOCK_SIZE;
	
		if (hex_data->flash_size % FLASH_BLOCK_SIZE) 
			block_count++;
	
		for (c = 0; c < block_count; c++) {
			if ((hex_data->flash_size - (c * FLASH_BLOCK_SIZE)) < FLASH_BLOCK_SIZE)
				len = hex_data->flash_size - (c * FLASH_BLOCK_SIZE);
			else 
				len = FLASH_BLOCK_SIZE;

			prepare_prog_data(FLASH_PROG, &hex_data->flash[c*FLASH_BLOCK_SIZE], len, 
						flash_addr, &liqrf);	
			fprintf(stderr, "Programming flash (%d)\n", len);
			usb_send_data(&liqrf);
			flash_addr += FLASH_ADDR_STEP;
		}
		/* send end progmode request */		
		enter_endprog_mode(&liqrf);
		usb_reset(liqrf.dev_handle);
	}
	
exit:	
	if (hex_data != NULL)
		hex_free_memory(hex_data);
	return 0;
}
