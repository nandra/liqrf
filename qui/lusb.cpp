#include <string.h>
#include "lusb.h"

/* constructor */
lusb::lusb()
{
    memset(this->rx_buff, 0, sizeof(this->rx_buff));
    memset(this->tx_buff, 0, sizeof(this->tx_buff));
    this->dev = NULL;
    this->dev_handle = NULL;
}

/* destructor */
lusb::~lusb()
{

}

/* observer to get device preset status */
int lusb::usb_dev_found()
{
    return found;
}

/* usb initialization */
void lusb::init_usb()
{
    found = 0;
    usb_bus = usb_get_busses();
    struct usb_device *device;
    usb_init();
    usb_find_busses();
    usb_find_devices();
    for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next) {
                for (device = usb_bus->devices; device; device = device->next) {
                        if ((device->descriptor.idVendor == IQRF_VENDOR_ID) &&
                            (device->descriptor.idProduct == IQRF_PRODUCT_ID)) {

                                if (device != NULL) {
                                    this->found = 1;
                                    this->dev = device;
                                }
                        }
                }
        }

}

/* opening usb device */
int lusb::open_usb()
{
    int ret_val = -1;

    if (this->dev != NULL) {
        this->dev_handle = usb_open(this->dev);
        if (this->dev_handle != NULL)
           ret_val = 0;
    }
    return ret_val;
}

/* receive data from endpoint */
int lusb::retrieve_packet()
{
    int ret_val = 0;

    ret_val = usb_interrupt_read(this->dev_handle, IN_EP_NR,
                                     this->rx_buff, this->rx_len,
                                     200);
    if (ret_val > 0)
        this->rx_len = ret_val;
    else
        this->rx_len = 0;

    return ret_val;
}

/* write data to endpoint */
int lusb::send_packet()
{
    int ret_val = 0;

    ret_val = usb_interrupt_write(this->dev_handle, OUT_EP_NR,
                                      this->tx_buff, this->tx_len,
                                      200);
    return ret_val;
}

/* write and read data to/from endpoint */
int lusb::send_receive_packet()
{
    int ret_val = 0;

    ret_val = this->send_packet();
    if (ret_val)
        ret_val = this->retrieve_packet();

    return ret_val;

}

/* set length for transmission */
void lusb::set_tx_len(int len = 0)
{
    this->tx_len = len;
}

/* get length of received data */
void lusb::set_rx_len(int len = 0)
{
    this->rx_len = len;
}

/* copy rx buffer */
int lusb::read_rx_buff(unsigned char * buff)
{
    memcpy(buff, this->rx_buff, this->rx_len);
    return this->rx_len;
}

/* write to tx buffer */
void lusb::write_tx_buff(unsigned char *buff, int len)
{
    memcpy(this->tx_buff, buff, len);
}

