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

#ifdef DEBUG_USB
int dbg_level = 3;
#else
int dbg_level = 0;
#endif


#ifdef DEBUG_USB
/* print some debugging info */
void print_dev_desc(struct usb_device *dev)
{
	printf("#	bDescriptorType: %d (0x%x)\n", dev->descriptor.bDescriptorType,
	       dev->descriptor.bDescriptorType);
	printf("#	bcdUSB: %d (0x%x)\n", dev->descriptor.bcdUSB,
	       dev->descriptor.bcdUSB);
	printf("#	bDeviceProtocol: %d (0x%x)\n", dev->descriptor.bDeviceProtocol,
	       dev->descriptor.bDeviceProtocol);
	printf("#	idVendor: %d (0x%x)\n", dev->descriptor.idVendor,
	       dev->descriptor.idVendor);
	printf("#	idProduct: %d (0x%x)\n", dev->descriptor.idProduct,
	       dev->descriptor.idProduct);
	printf("#	bcdDevice: %d (0x%x)\n", dev->descriptor.bcdDevice,
	       dev->descriptor.bcdDevice);
	printf("#	iManufacturer: %d (0x%x)\n", dev->descriptor.iManufacturer,
	       dev->descriptor.iManufacturer);
	printf("#	iProduct: %d (0x%x)\n", dev->descriptor.iProduct,
	       dev->descriptor.iProduct);
	printf("#	bNumConfigurations: %d (0x%x)\n",dev->descriptor.bNumConfigurations,
		dev->descriptor.bNumConfigurations);
}

#endif

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
#ifdef DEBUG_USB
				if (dev != NULL)
					print_dev_desc(dev);
#endif	
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

	printf("Written data len = %d:\n", tx_len);
	for (i = 0; i < tx_len; i++)
		printf("%02X ", (unsigned char)tx_buff[i]);
	printf("\n");

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
	
// #ifdef DEBUG_USB
	printf("Readed string len = %d:\n", ret_val);
	for (i = 0; i < ret_val; i++) 
		printf("%02X ", (unsigned char)rx_buff[i]);
	
	printf("\n");
// #endif
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
exit:
	return ret_val;
}