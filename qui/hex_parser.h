#ifndef HEX_PARSER_H
#define HEX_PARSER_H

// define supported hex format
#define	HEX88

#ifdef	HEX88
#define USR_EEPROM_MAX_SIZE	160*2
#define APP_EEPROM_MAX_SIZE	32*2
#define FLASH_MAX_SIZE		1000

#define FLASH_START_ADR		0x1c00
#define FLASH_END_ADR		FLASH_START_ADR+FLASH_MAX_SIZE
#define USR_EEPROM_START_ADR	0X4200
#define	USR_EEPROM_END_ADR	USR_EEPROM_START_ADR+USR_EEPROM_MAX_SIZE
#define APP_EEPROM_START_ADR	0x4340
#define APP_EEPROM_END_ADR	APP_EEPROM_START_ADR+APP_EEPROM_MAX_SIZE
#endif

// types of hex records
typedef enum {  DATA,
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
        bool read_file();
        unsigned char app_eeprom[APP_EEPROM_MAX_SIZE];
        unsigned char usr_eeprom[USR_EEPROM_MAX_SIZE];
        unsigned char flash[FLASH_MAX_SIZE];
        unsigned int app_eeprom_size;
        unsigned int usr_eeprom_size;
        unsigned int flash_size;
        QString hexfile;
};

//extern int verbose;
//int verbose = 1;
#endif // HEX_PARSER_H
