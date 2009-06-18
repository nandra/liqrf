#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QFile>
#include "hex_parser.h"

#define MAX_DATA 32

/* debugging with global verbose */
#define debug_parser(format, arg...) \
        //if (verbose) {\
                printf("parser:" format , ## arg); \
        //} else {}\


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
   read and parse data from hex file
   return: true if read was OK
           false if read was NOK
*/
bool hex_parser::read_file()
{
    FILE *hex;
    int	len, adr, type;
    unsigned int data;	// temporary buffer for data
    int i;

    // open hex file
    hex = fopen(hexfile.toLatin1(), "r");
    if (hex == NULL) {
        fprintf(stderr, "Cannot open file %s\n", hexfile);
        return false;
    }

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
