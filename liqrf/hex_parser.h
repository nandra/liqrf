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
#define USR_EEPROM_MAX_SIZE	160*2
#define APP_EEPROM_MAX_SIZE	32*2 
#define FLASH_MAX_SIZE		1000

#define FLASH_START_ADR		0x1c00
#define FLASH_END_ADR		FLASH_START_ADR+FLASH_MAX_SIZE
#define USR_EEPROM_START_ADR	0X4200
#define	USR_EEPROM_END_ADR	USR_EEPROM_START_ADR+USR_EEPROM_MAX_SIZE
#define APP_EEPROM_START_ADR	0x4340
#define APP_EEPROM_END_ADR	APP_EEPROM_START_ADR+APP_EEPROM_MAX_SIZE
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
	unsigned char app_eeprom[APP_EEPROM_MAX_SIZE];
	unsigned char usr_eeprom[USR_EEPROM_MAX_SIZE];
	unsigned char flash[FLASH_MAX_SIZE];
	unsigned int app_eeprom_size;
	unsigned int usr_eeprom_size;
	unsigned int flash_size;
}program_data;

program_data *hex_get_data(char *hexfile);

#endif