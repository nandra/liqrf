#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "iqrf_dev.h"


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

class programmer {
public:
    programmer();
    ~programmer();
    iqrf_dev *dev;
    void enter_prog_mode();
    int request_module_id(void);
    int get_module_id(void);
    unsigned char module_id[20];
private:

};


#endif // PROGRAMMER_H
