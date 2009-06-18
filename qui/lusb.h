#ifndef USB_H
#define USB_H

#include <usb.h>

/* usb device has only 1 configuration
 * and only 1 interface which consist
 * from 2 interrupt endpoints */

#define IQRF_VENDOR_ID 0x04D8
#define IQRF_PRODUCT_ID 0x000C
/* maximum length for rx and tx buffer */
#define BUF_LEN 64

/* endpoint numbers */
#define OUT_EP_NR (0x01)
#define IN_EP_NR (0x81)

#define NO_MODULE_ON_USB        0xFF    // SPI not working (HW error)
#define SPI_DISABLED            0x00    // SPI not working (disabled)
#define SPI_CRCM_OK             0x3F    // SPI not ready (full buffer, last CRCM ok)
#define SPI_CRCM_ERR            0x3E    // SPI not ready (full buffer, last CRCM error)
#define COMMUNICATION_MODE      0x80    // SPI ready (communication mode)
#define PROGRAMMING_MODE        0x81    // SPI ready (programming mode)
#define DEBUG_MODE              0x82    // SPI ready (debugging mode)
#define SPI_SLOW_MODE           0x83    // SPI is not working on the background - Slow mode
#define SPI_USER_STOP           0x07    // state after stopSPI();
#define SPI_DATA_READY          0x40    // data ready

class lusb{
    public:
        int status;
        lusb();
        ~lusb();
        int usb_dev_found();
        void init_usb();
        int open_usb();
        int send_receive_packet();
        void set_tx_len(int);
        void set_rx_len(int);
        int read_rx_buff(unsigned char *buff);
        void write_tx_buff(unsigned char *buff, int len);
    private:
        int send_packet();
        int retrieve_packet();
        struct usb_bus *usb_bus;
        struct usb_dev_handle *dev_handle;
        struct usb_device *dev;

        int found;
        char tx_buff[BUF_LEN], rx_buff[BUF_LEN];
        char tx_len, rx_len;
};

#endif // USB_H
