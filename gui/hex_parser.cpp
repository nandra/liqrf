#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QFile>
#include <QDebug>
#include "hex_parser.h"

#define MAX_DATA 32
#define DEBUG

/* debugging with global verbose */
#ifdef DEBUG
#define debug_parser(format, arg...) printf("parser:" format, ## arg);
#else
#define debug_parser(format, arg...) {}
#endif

hex_parser::hex_parser()
{
    app_eeprom_size = 0;
    usr_eeprom_size = 0;
    flash_size = 0;
}

hex_parser::~hex_parser()
{

}

/*
   setup hex format
   return: true if setup was OK
           false if setup was NOK
*/
bool hex_parser::set_hex_format(hex_format format)
{
    if (format == MCU_16F88) {
        USR_EEPROM_MAX_SIZE = 160*2;
        APP_EEPROM_MAX_SIZE = 32*2;
        FLASH_MAX_SIZE = 1024;

        FLASH_START_ADR = 0x1c00;
        FLASH_END_ADR = FLASH_START_ADR+FLASH_MAX_SIZE;
        USR_EEPROM_START_ADR = 0X4200;
        USR_EEPROM_END_ADR = USR_EEPROM_START_ADR+USR_EEPROM_MAX_SIZE;
        APP_EEPROM_START_ADR = 0x4340;
        APP_EEPROM_END_ADR = APP_EEPROM_START_ADR+APP_EEPROM_MAX_SIZE;

    } else if (format == MCU_16F886) {
        USR_EEPROM_MAX_SIZE = 160*2;
        APP_EEPROM_MAX_SIZE = 32*2;
        FLASH_MAX_SIZE = 2048;

        FLASH_START_ADR = 0x3800;
        FLASH_END_ADR = FLASH_START_ADR+FLASH_MAX_SIZE;
        USR_EEPROM_START_ADR = 0X4200;
        USR_EEPROM_END_ADR = USR_EEPROM_START_ADR+USR_EEPROM_MAX_SIZE;
        APP_EEPROM_START_ADR = 0x4340;
        APP_EEPROM_END_ADR = APP_EEPROM_START_ADR+APP_EEPROM_MAX_SIZE;

    } else
        //unsupported hex format
        return false;

    return true;
}


/*
   read and parse data from hex file
   parameter: format of hex file
   return: true if read was OK
           false if read was NOK
*/
bool hex_parser::read_file(hex_format format)
{
    FILE *hex;
    unsigned int len, adr, type;
    unsigned int data;	// temporary buffer for data
    unsigned int i;

    // reset parser
    app_eeprom_size = 0;
    usr_eeprom_size = 0;
    flash_size = 0;

    // open hex file
    hex = fopen(hexfile.toLatin1(), "r");
    if (hex == NULL) {
        qDebug() <<"Cannot open file" << hexfile.toLatin1();
        return false;
    }

    // setup hex format
    if (!this->set_hex_format(format))
        return false;

    // read hex file
    while (1) {
        if (fscanf(hex, ":%2x%4x%2x", &len, &adr, &type) != 3) {
            goto read_err;
        }
        debug_parser("lenght is %d, address is %d, type is %d\n", len, adr, type);

        switch (type) {
        case DATA:
            // read data to buffer
            for (i=0; i<len; i++) {
                if (fscanf(hex, "%2x", &data) != 1){
                    goto read_err;
                }
                if ((adr >= FLASH_START_ADR) && (adr < FLASH_END_ADR)) {
                    flash[flash_size] = data;

                    debug_parser("flash[%d] = 0x%x\n", flash_size,
                    flash[flash_size]);

                    flash_size++;
                } else if ((adr >= USR_EEPROM_START_ADR) &&
                            (adr < USR_EEPROM_END_ADR)) {
                    usr_eeprom[usr_eeprom_size] = data;

                    debug_parser("usr_eeprom[%d] = 0x%x\n", usr_eeprom_size, usr_eeprom[usr_eeprom_size]);

                    usr_eeprom_size++;
                    fscanf(hex,"%2x", &data); // skip every second byte (=0x00)
                    i++;
                } else if ((adr >= APP_EEPROM_START_ADR) &&
                            (adr < APP_EEPROM_END_ADR)) {
                    app_eeprom[app_eeprom_size] = data;

                    debug_parser("app_eeprom[%d] = 0x%x\n", app_eeprom_size, app_eeprom[app_eeprom_size]);

                    app_eeprom_size++;
                    fscanf(hex,"%2x", &data); // skip every second byte (=0x00)
                    i++;
                }
                // other addresses are ignored
        }
                debug_parser("\n");
                break;
        case END_OF_FILE:
                debug_parser("EEPROM size: %d\n", app_eeprom_size + usr_eeprom_size);
                debug_parser("FLASH size: %d\n", flash_size);
                fclose(hex);
                return true;
        default:
                fprintf(stderr, "Unsupported type of hex format.\n");
                goto read_err;
        }

        // go to end of line
        while (getc(hex) != '\n')	// end of file check?
                ;

    }

    read_err:
        fclose(hex);
        return false;
}
