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
#include <usb.h>
#include <string.h>

#include "iqrf.h"

#define DEBUG

#ifdef DEBUG
int dbg_level = 3;
#else
int dbg_level = 0;
#endif

/* object for holding global variables */
struct liqrf_obj {
	struct usb_device *dev;
	struct usb_dev_handle *dev_handle;
	unsigned char rx_buff[BUF_LEN];
	unsigned char tx_buff[BUF_LEN];
	int tx_len;
	int rx_len;
};

unsigned char master_only_read, spi_check = 0;

struct liqrf_obj iqrf_dev;
struct liqrf_obj *iqrf = &iqrf_dev;

/* device initialization */
static struct usb_device *liqrf_device_init(void)
{
	struct usb_bus *usb_bus;
	struct usb_device *dev;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next) {
		for (dev = usb_bus->devices; dev; dev = dev->next) {
			if ((dev->descriptor.idVendor == IQRF_VENDOR_ID) &&
			    (dev->descriptor.idProduct == IQRF_PRODUCT_ID))
				return dev;
		}
	}
	return NULL;
}

/* write to output endpoint and read
   data from input endpoint */
static int liqrf_send_receive_packet(struct liqrf_obj *iqrf)
{
	int ret_val = 0, i = 0;

	ret_val = usb_interrupt_write(iqrf->dev_handle, OUT_EP_NR,
				      (char *)iqrf->tx_buff, iqrf->tx_len,
				      32000);

	if (ret_val < 0) {
		fprintf(stderr, "Can't write to output endpoint\n");
		goto exit;
	}
#ifdef DEBUG
	for (i = 0; i < iqrf->tx_len; i++)
		printf("Written string: %X", iqrf->tx_buff[i]);

	printf("\n");
#endif
	ret_val = usb_interrupt_read(iqrf->dev_handle, IN_EP_NR,
				     (char *)iqrf->rx_buff, iqrf->rx_len,
				     32000);

	if (ret_val < 0) {
		fprintf(stderr, "Can't read from input endpoint\n");
		goto exit;
	}
#ifdef DEBUG
	for (i = 0; i < ret_val; i++)
		printf("Readed string: %X", iqrf->rx_buff[i]);

	printf("\n");
#endif
	return 0;
exit:
	return ret_val;
}

/* parse status and check if data are ready */
enum data_status liqrf_check_data(unsigned char spi_status)
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
unsigned char check_CRC_rx(unsigned char *buff, int len, int PTYPE, int DLEN)
{
	unsigned char i, crc_val;

	crc_val = 0x5F ^ PTYPE;

	for (i = 2; i < len; i++)
		crc_val ^= buff[i];

	if (buff[DLEN + 2] == crc_val)
		return 1;

	return 0;
}

/* read and write to module via SPI */
int liqrf_read_write(struct liqrf_obj *iqrf, int spi_check, int DLEN)
{
	int PTYPE = 0;
	memset(iqrf->rx_buff, 0, sizeof(iqrf->rx_buff));

	/* prepare tx packet */
	/* see SPI User Manual (www.iqrf.org) */
	if (!spi_check) {
		/* Read/Write buffer COM in TR module */
		iqrf->tx_buff[1] = SPI_R_W;
		/* read buffer COM in TR module (clear bit7)
		 * write buffer COM in TR module (set bit7)
		 */
		if (master_only_read)
			PTYPE = (DLEN & 0x7F);
		else
			PTYPE = (DLEN | 0x80);

		iqrf->tx_buff[2] = PTYPE;
		iqrf->tx_buff[DLEN + 3] = count_CRC_tx(iqrf->tx_buff, DLEN + 3);
	}
	/* command for CK-USB-xx "send this data to TR module via SPI */
	iqrf->tx_buff[0] = CMD_FOR_CK;

	iqrf->tx_len = DLEN + 5;
	iqrf->rx_len = DLEN + 4;

	liqrf_send_receive_packet(iqrf);

	/* this is maybee not necessary */
	/* CloseUSB(); */
	memset(iqrf->tx_buff, 0, sizeof(iqrf->tx_buff));

	if (!spi_check)
		/* check CRC or received packet */
		if (!check_CRC_rx(iqrf->rx_buff, DLEN + 2, PTYPE, DLEN))
			return 0;

	spi_check = 0;

	return 1;
}

int main()
{
	iqrf->dev = liqrf_device_init();

	if (iqrf->dev == NULL) {
		fprintf(stderr, "Could not init device\n");
		goto exit;
	}

	usb_set_debug(dbg_level);

	iqrf->dev_handle = usb_open(iqrf->dev);

	if (iqrf->dev_handle == NULL) {
		fprintf(stderr, "Could not open device\n");
		goto exit;
	}
	/* usb device is called periodically 
	 * by timer 
	 */
exit:

	return 0;
}
