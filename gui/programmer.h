#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "iqrf_dev.h"
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
#define UNKNOWN 0xA2

/* flash and eeprom data types */
#define EEPROM_APP 0x01
#define EEPROM_USER 0x02
#define FLASH_PROG 0x03

#define FLASH_BLOCK_SIZE (32)
#define APP_EEPROM_BASE_ADDR (0xA0)
#define USR_EEPROM_BASE_ADDR (0x00)
#define FLASH_BASE_ADDR (0x0E00)
#define FLASH_ADDR_STEP (0x10)

class programmer {
public:
    programmer();
    ~programmer();
    iqrf_dev *dev;
    hex_parser *parser;
    void enter_prog_mode();
    void enter_endprog_mode();
    int request_module_id(void);
    int get_module_id(void);
    unsigned char module_id[20];
    int send_prog_data(int data_type, unsigned char *data, int data_len,
                        unsigned short addr);
    int reset_module();
    void release();
private:

};


#endif // PROGRAMMER_H