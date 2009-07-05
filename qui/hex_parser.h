#ifndef HEX_PARSER_H
#define HEX_PARSER_H

#include <QString>

// supported hex formats
typedef enum {
    HEX88,
    HEX886
}hex_format;

// types of hex records
typedef enum {
    DATA,
    END_OF_FILE,
    EXTENDED_SEGMENT_ADR,
    START_SEGMENT_ADR,
    EXTENDED_LINEAR_ADR,
    START_LINEAR_ADR
}hex_type;


// hex parser class definition
class hex_parser{
    public:
        hex_parser();
        ~hex_parser();
        bool read_file(hex_format format);
        unsigned char app_eeprom[320];
        unsigned char usr_eeprom[64];
        unsigned char flash[2048];
        unsigned int app_eeprom_size;
        unsigned int usr_eeprom_size;
        unsigned int flash_size;
        QString hexfile;
    private:
        unsigned int USR_EEPROM_MAX_SIZE;
        unsigned int APP_EEPROM_MAX_SIZE;
        unsigned int FLASH_MAX_SIZE;
        unsigned int FLASH_START_ADR;
        unsigned int FLASH_END_ADR;
        unsigned int USR_EEPROM_START_ADR;
        unsigned int USR_EEPROM_END_ADR;
        unsigned int APP_EEPROM_START_ADR;
        unsigned int APP_EEPROM_END_ADR;
        bool set_hex_format(hex_format format);
};

//extern int verbose;
//int verbose = 1;
#endif // HEX_PARSER_H
