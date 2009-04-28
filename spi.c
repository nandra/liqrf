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
#include <string.h>
#include "spi.h"

#define debug_spi(format, arg...) \
	if (verbose) {\
		printf("spi:" format , ## arg); \
	} else {}\

/* parse status and check if data are ready */
enum data_status spi_check_data(unsigned char spi_status)
{
	enum data_status stat = DATA_NOT_READY;

	switch (spi_status) {
	case NO_MODULE_ON_USB:
	case SPI_DISABLED:
		debug_spi("SPI not working (disabled)\n");
		break;

	case SPI_CRCM_OK:
		debug_spi("Module not ready (full buffer, last CRCM ok)\n");
		break;

	case SPI_CRCM_ERR:
		debug_spi("Module not ready (full buffer, last CRCM error)\n");
		break;

	case COMMUNICATION_MODE:
		debug_spi("Module ready - communication mode\n");
		break;

	case PROGRAMMING_MODE:
		debug_spi("Module ready - programming mode\n");
		break;

	case DEBUG_MODE:
		debug_spi("Module ready - debugging mode\n");
		break;
	default:
		if ((spi_status & 0xC0) == 0x40)
			stat = DATA_READY;
	}

	return stat;
}

/* count CRC for transmission */
unsigned char count_crc_tx(unsigned char *buff, int len)
{
	unsigned char crc_val;
	int i = 0;

	crc_val = 0x5F;

	for (i = 0; i < len; i++) 
		crc_val ^= buff[i];

	return crc_val;
}

/* count CRC for received buffer */
unsigned char check_crc_rx(unsigned char *buff, int PTYPE, int len)
{
	unsigned char i, crc_val;

	crc_val = 0x5F ^ PTYPE;

	for (i = 0; i < len; i++) {
		debug_spi("crc = %x, buff[%d]=%x\n", crc_val, i, buff[i]);
		crc_val ^= buff[i];
	}
	
	if (buff[len] == crc_val)
		return 1;
	
	fprintf(stderr, "Wrong checksum!\n");
	return 0;
}

