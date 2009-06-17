#include "lusb.h"

lusb::lusb()
{
    found = 0;
    usb_bus = usb_get_busses();

    usb_init();
    usb_find_busses();
    usb_find_devices();
    for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next) {
                for (dev = usb_bus->devices; dev; dev = dev->next) {
                        if ((dev->descriptor.idVendor == IQRF_VENDOR_ID) &&
                            (dev->descriptor.idProduct == IQRF_PRODUCT_ID)) {

                                if (dev != NULL) {
                                    found = 1;
                                }
                        }
                }
        }

}

lusb::~lusb()
{

}

int lusb::usb_dev_found()
{
    return found;
}

