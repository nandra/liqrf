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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "hex_parser.h"

#define MAX_DATA 32


/*
   read and parse data from hex file
   parameters: hexfile - file name
   return: pointer to program data object
	   NULL in case of problem
   note: created object program_data should
	  be freed later in program
*/
program_data *hex_get_data(char *hexfile)
{
	program_data *prog_obj;
	FILE *hex;
	int	len, adr, type;
	unsigned int data;	// temporary buffer for data
	int i;

	// init program object
	prog_obj = (program_data *) malloc(sizeof(program_data));
	if (prog_obj == NULL) {
		fprintf(stderr, "Not enough memory.\n");
		return NULL;
	}
	prog_obj->eeprom_size = 0;
	prog_obj->flash_size = 0;

	// open hex file
	hex = fopen(hexfile, "r");
	if (hex == NULL) {
		fprintf(stderr, "Cannot open file %s\n", hexfile);
		return NULL;
	}

	// read hex file
	while (1) {
		if (fscanf(hex, ":%2x%4x%2x", &len, &adr, &type) != 3) {
			goto read_err;
		}
		printf("lenght is %d, address is %d, type is %d\n", len, adr, type);

		switch (type) {
		case DATA:	
			// read data to buffer
			for (i=0; i<len; i++) {
				if (fscanf(hex, "%2x", &data) != 1){
					goto read_err;
				}
				printf("%d ", data);
				if ((adr >= FLASH_START_ADR) && (adr < FLASH_END_ADR)) {
					prog_obj->flash[prog_obj->flash_size] = data;

					printf("flash[%d] = 0x%x\n", prog_obj->flash_size, 
						prog_obj->flash[prog_obj->flash_size]);

					prog_obj->flash_size++;
				} else if ((adr >= EEPROM_START_ADR) && (adr < EEPROM_END_ADR)) {
					prog_obj->eeprom[prog_obj->eeprom_size] = data;

					printf("eeprom[%d] = 0x%x\n", prog_obj->eeprom_size, 
						prog_obj->eeprom[prog_obj->eeprom_size]);

					prog_obj->eeprom_size++;
					fscanf(hex,"%2x", &data); // skip every second byte (=0x00)
					i++;
				}
				// other addresses are ignored
			}
			printf("\n");
			break;
		case END_OF_FILE:
			fclose(hex);
			return prog_obj;
		default:
			fprintf(stderr, "Unsupported type of hex format.\n");
			goto read_err;
		}

		// go to end of line
		while (getc(hex) != '\n')	// end of file check?
			;

	}

	read_err:
		fclose(hex);
		free(prog_obj);
		return NULL;
}