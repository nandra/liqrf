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
#ifndef USB_H
#define USB_H

#include <usb.h>

#define DEBUG_USB

/* usb device has only 1 configuration 
   and only 1 interface which consist 
   from 2 interrupt endpoints 
  
   vendor id and product id only for 
   CK USB2 (CK USB3 not supported yet)
*/

#define IQRF_VENDOR_ID (0x04D8)
#define IQRF_PRODUCT_ID (0x000C)

/* endpoint numbers */
#define OUT_EP_NR (0x01)
#define IN_EP_NR (0x81)

/* exports */
struct usb_device *liqrf_device_init(void);
int liqrf_send_receive_packet(struct usb_dev_handle *dev_handle, 
				char *tx_buff, int tx_len, char *rx_buff, int rx_len);
#define liqrf_device_open(dev) usb_open(dev)

#endif /* USB_H */