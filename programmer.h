#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "core_class/iqrf_dev.h"
#include "hex_parser.h"

/* every usb prog. write has following entries
   CMD_PROG, EEPROM_DATA or FLASH_DATA,
   UNKNOWN, address, data, checksum
*/
#define CMD_ENTER_PROG 0x02
#define UNKNOWN_PROG 0xE6

#define CMD_PROG 0x07
#define EEPROM_DATA 0xF3
#define FLASH_DATA 0xF6
#define UNKNOWN_DATA 0xA2

/* flash and eeprom data types */
#define EEPROM_APP 0x01
#define EEPROM_USER 0x02
#define FLASH_PROG 0x03

#define FLASH_BLOCK_SIZE (32)
#define APP_EEPROM_BASE_ADDR (0xA0)
#define USR_EEPROM_BASE_ADDR (0x00)
#define FLASH_BASE_ADDR_16F88 (0x0E00)
#define FLASH_BASE_ADDR_16F886 (0x1C00)
#define FLASH_ADDR_STEP (0x10)

class programmer {
public:
    programmer();
    ~programmer();

    hex_parser *parser;
    void enter_prog_mode();
    void enter_endprog_mode();
    int request_module_id(void);
    int get_module_id(void);
    unsigned char module_id[20];
    unsigned char spi_tx_data[35];
    int send_prog_data(int data_type, unsigned char *data, int data_len,
                        unsigned short addr);
    int reset_module();
    void release();
    int write_read_spi_data(unsigned char *data, int data_len, bool write_read);
    int write_read_test_spi_data(unsigned char *data, int data_len);

    int init();
    int get_status();
    void reset();
    int crc(unsigned char *buff, int len);

private:
    iqrf_dev *dev;
    unsigned char usr_eeprom;
};


#endif // PROGRAMMER_H
