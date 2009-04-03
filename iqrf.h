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

#ifndef _IQRF_H
#define _IQRF_H

#define BUF_LEN 64


/* usb device has only 1 configuration 
 * and only 1 interface which consist 
 * from 2 interrupt endpoints */

#define IQRF_VENDOR_ID 0x04D8
#define IQRF_PRODUCT_ID 0x000C

#define OUT_EP_NR (0x01)
#define IN_EP_NR (0x81)

#define buffer_clear(buff, len) memset(buff, 0, len)

#define SPI_CHECK                       0x00
#define SPI_R_W                         0xF0    //MASTER will write/read module (buffer COM)

#define CMD_FOR_CK                      0x01    //command for CK

enum data_status {
	DATA_NOT_READY,
	DATA_READY,
};

enum spi_check_mode {
	SPI_NOT_CHECKING,
	SPI_CHECKING,
};
/* SPI status */

#define NO_MODULE_ON_USB        0xFF    // SPI not working (HW error)
#define SPI_DISABLED            0x00    // SPI not working (disabled)
#define SPI_CRCM_OK             0x3F    // SPI not ready (full buffer, last CRCM ok)
#define SPI_CRCM_ERR            0x3E    // SPI not ready (full buffer, last CRCM error)
#define COMMUNICATION_MODE      0x80    // SPI ready (communication mode)
#define PROGRAMMING_MODE        0x81    // SPI ready (programming mode)
#define DEBUG_MODE              0x82    // SPI ready (debugging mode)
#define SPI_SLOW_MODE           0x83    // SPI is not working on the background - Slow mode
#define SPI_USER_STOP           0x07    // state after stopSPI();


#endif /* _IQRF_H */