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
#ifndef HEX_PARSER_H
#define HEX_PARSER_H

// define supported hex format
#define	HEX88

#ifdef	HEX88
#define EEPROM_MAX_SIZE	32
#define FLASH_MAX_SIZE	1000

#define FLASH_START_ADR		0x1c00
#define FLASH_END_ADR		0x4300	
#define EEPROM_START_ADR	0x4340
#define EEPROM_END_ADR		0xffff
#endif

#define hex_free_memory(x) free(x)

// types of hex records
typedef enum {  DATA, 
		END_OF_FILE, 
		EXTENDED_SEGMENT_ADR, 
		START_SEGMENT_ADR, 
		EXTENDED_LINEAR_ADR, 
		START_LINEAR_ADR 
}hex_type;

typedef struct {
	char eeprom[EEPROM_MAX_SIZE];
	char flash[FLASH_MAX_SIZE];
	unsigned int eeprom_size;
	unsigned int flash_size;
}program_data;

program_data *hex_get_data(char *hexfile);

#endif