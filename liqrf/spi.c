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


/* parse status and check if data are ready */
enum data_status spi_check_data(unsigned char spi_status)
{
	enum data_status stat = DATA_NOT_READY;

	switch (spi_status) {
	case NO_MODULE_ON_USB:
	case SPI_DISABLED:
		fprintf(stderr, "SPI not working (disabled)\n");
		break;

	case SPI_CRCM_OK:
		fprintf(stderr,
			"Module not ready (full buffer, last CRCM ok)\n");
		break;

	case SPI_CRCM_ERR:
		fprintf(stderr,
			"Module not ready (full buffer, last CRCM error)\n");
		break;

	case COMMUNICATION_MODE:
		fprintf(stderr, "Module ready - communication mode\n");
		break;

	case PROGRAMMING_MODE:
		fprintf(stderr, "Module ready - programming mode\n");
		break;

	case DEBUG_MODE:
		fprintf(stderr, "Module ready - debugging mode\n");
		break;
	default:
		if ((spi_status & 0xC0) == 0x40)
			stat = DATA_READY;
	}

	return stat;
}

/* count CRC for transmission */
unsigned char count_CRC_tx(unsigned char *buff, int len)
{
	unsigned char crc_val;
	int i = 0;

	crc_val = 0x5F;

	for (i = 1; i < len; i++)
		crc_val ^= buff[i];

	return crc_val;
}

/* count CRC for received buffer */
unsigned char check_CRC_rx(unsigned char *buff, int PTYPE, int DLEN)
{
	unsigned char i, crc_val;

	crc_val = 0x5F ^ PTYPE;

	for (i = 2; i < DLEN + 2; i++)
		crc_val ^= buff[i];

	if (buff[DLEN + 2] == crc_val)
		return 1;

	return 0;
}

/* read and write to module via SPI */
// int liqrf_read_write(struct liqrf_obj *iqrf, int spi_check, int DLEN)
// {
// 	int PTYPE = 0;
// 	memset(iqrf->rx_buff, 0, sizeof(iqrf->rx_buff));
// 
// 	if (spi_check) {
// 		iqrf->tx_buff[1] = 0x00;
// 	} else {
// 		if (iqrf->master_only_read)
// 			PTYPE = DLEN & 0x7F;
// 		else
// 			PTYPE = DLEN | 0x80;
// 
// 		iqrf->tx_buff[1] = SPI_R_W;
// 		iqrf->tx_buff[2] = PTYPE;
// 		iqrf->tx_buff[DLEN + 3] = count_CRC_tx(iqrf->tx_buff, DLEN + 3);
// 	}
// 	iqrf->tx_buff[0] = CMD_FOR_CK;
// 
// 	iqrf->tx_len = DLEN + 5;
// 	iqrf->rx_len = DLEN + 4;
// 
// 	if (liqrf_send_receive_packet(iqrf))
// 		goto err;
// 
// 	memset(iqrf->tx_buff, 0, sizeof(iqrf->tx_buff));
// 
// 	if (!spi_check)
// 		/* check CRC for received packet */
// 		if (!check_CRC_rx(iqrf->rx_buff, PTYPE, DLEN))
// 			return 0;
// err:
// 	return 1;
// }