#ifndef USB_H
#define USB_H

#include <usb.h>

/* usb device has only 1 configuration
 * and only 1 interface which consist
 * from 2 interrupt endpoints */

#define IQRF_VENDOR_ID 0x04D8
#define IQRF_PRODUCT_ID 0x000C

class lusb{
    public:
        lusb();
        ~lusb();
        int usb_dev_found();
    private:
        struct usb_bus *usb_bus;
        struct usb_device *dev;
        int found;
};

#endif // USB_H
