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
#include <unistd.h>

#include "liqrf.h"

void print_help(void)
{
	fprintf(stderr, "Using ./liqrf -f <hex file> [-v]\n");		
	fprintf(stderr, "-f <hex file> - hex file for loading \n");
	fprintf(stderr, "-v - verbose output\n");
}


int main (int argc, char **argv)
{
	int verbose = 0;
	struct liqrf_obj liqrf;
	char *hex_file = NULL;
	int c;
	program_data *hex_data;
	opterr = 0;
	
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

	for (c= 0; c < EEPROM_MAX_SIZE; c++)
		printf("EEPROM[%d] = 0x%X\n",c, hex_data->eeprom[c]);
exit:	
	return 0;
}
