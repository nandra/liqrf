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

#include "usb.h"

#define debug_usb(format, arg...) \
	if (verbose) {\
		printf("usb:" format , ## arg); \
	} else {}\
	 	 
#define debug_usb_notext(format, arg...) \
	if (verbose) {\
		printf(format , ## arg); \
	} else {}\

/* print some debugging info */
void print_dev_desc(struct usb_device *dev)
{
	debug_usb("#	bDescriptorType: %d (0x%x)\n", dev->descriptor.bDescriptorType,
	       dev->descriptor.bDescriptorType);
	debug_usb("#	bcdUSB: %d (0x%x)\n", dev->descriptor.bcdUSB,
	       dev->descriptor.bcdUSB);
	debug_usb("#	bDeviceProtocol: %d (0x%x)\n", dev->descriptor.bDeviceProtocol,
	       dev->descriptor.bDeviceProtocol);
	debug_usb("#	idVendor: %d (0x%x)\n", dev->descriptor.idVendor,
	       dev->descriptor.idVendor);
	debug_usb("#	idProduct: %d (0x%x)\n", dev->descriptor.idProduct,
	       dev->descriptor.idProduct);
	debug_usb("#	bcdDevice: %d (0x%x)\n", dev->descriptor.bcdDevice,
	       dev->descriptor.bcdDevice);
	debug_usb("#	iManufacturer: %d (0x%x)\n", dev->descriptor.iManufacturer,
	       dev->descriptor.iManufacturer);
	debug_usb("#	iProduct: %d (0x%x)\n", dev->descriptor.iProduct,
	       dev->descriptor.iProduct);
	debug_usb("#	bNumConfigurations: %d (0x%x)\n",dev->descriptor.bNumConfigurations,
		dev->descriptor.bNumConfigurations);
}

/* device initialization */
struct usb_device *liqrf_device_init(void)
{
	struct usb_bus *usb_bus;
	struct usb_device *dev;

	usb_bus = usb_get_busses();

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next) {
		for (dev = usb_bus->devices; dev; dev = dev->next) {
			if ((dev->descriptor.idVendor == IQRF_VENDOR_ID) &&
			    (dev->descriptor.idProduct == IQRF_PRODUCT_ID)) {

				if (dev != NULL)
					print_dev_desc(dev);
	
				return dev;
			}
		}
	}

	return NULL;
}

/* write to output endpoint and read
   data from input endpoint 
 */
int send_receive_packet(struct usb_dev_handle *dev_handle, 
				char *tx_buff, int tx_len, char *rx_buff, int rx_len)
{
	int ret_val = 0, i = 0;

	debug_usb("Written data len = %d:\n", tx_len);
	for (i = 0; i < tx_len; i++)
		debug_usb_notext("%02X ", (unsigned char)tx_buff[i]);
	debug_usb_notext("\n");

	ret_val = usb_interrupt_write(dev_handle, OUT_EP_NR,
				      tx_buff, tx_len,
				      32000);

	if (ret_val < 0) {
		fprintf(stderr, "Can't write to output endpoint\n");
		goto exit;
	}

	ret_val = usb_interrupt_read(dev_handle, IN_EP_NR,
				     rx_buff, rx_len,
				     32000);
	
	if (ret_val < 0) {
		fprintf(stderr, "Can't read from input endpoint\n");
		goto exit;
	}

	debug_usb("Readed string len = %d:\n", ret_val);
	for (i = 0; i < ret_val; i++) 
		debug_usb_notext("%02X ", (unsigned char)rx_buff[i]);
	
	debug_usb_notext("\n");

	return 0;
exit:
	return ret_val;
}

/* write to output endpoint */
int send_packet(struct usb_dev_handle *dev_handle, char *tx_buff, int tx_len)
{
	int ret_val = 0, i = 0;

	ret_val = usb_interrupt_write(dev_handle, OUT_EP_NR,
				      tx_buff, tx_len,
				      32000);

	if (ret_val < 0) {
		fprintf(stderr, "Can't write to output endpoint\n");
		goto exit;
	}
	debug_usb("Written data len = %d:\n", tx_len);
	for (i = 0; i < tx_len; i++)
		debug_usb_notext("%02X ", (unsigned char)tx_buff[i]);
	debug_usb_notext("\n");
exit:
	return ret_val;
}