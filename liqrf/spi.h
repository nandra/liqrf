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
#ifndef SPI_H
#define SPI_H

enum data_status {
        DATA_NOT_READY,
        DATA_READY,
};


#define NO_MODULE_ON_USB        0xFF    // SPI not working (HW error)
#define SPI_DISABLED            0x00    // SPI not working (disabled)
#define SPI_CRCM_OK             0x3F    // SPI not ready (full buffer, last CRCM ok)
#define SPI_CRCM_ERR            0x3E    // SPI not ready (full buffer, last CRCM error)
#define COMMUNICATION_MODE      0x80    // SPI ready (communication mode)
#define PROGRAMMING_MODE        0x81    // SPI ready (programming mode)
#define DEBUG_MODE              0x82    // SPI ready (debugging mode)
#define SPI_SLOW_MODE           0x83    // SPI is not working on the background - Slow mode
#define SPI_USER_STOP           0x07    // state after stopSPI();

unsigned char count_crc_tx(unsigned char *buff, int len);
unsigned char check_crc_rx(unsigned char *buff, int PTYPE, int len);

#endif /* SPI_H */