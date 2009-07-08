#include <stdio.h>
#include <string.h>
#include "programmer.h"

programmer::programmer()
{
    this->dev = new iqrf_dev;
    this->parser = new hex_parser;
}

programmer::~programmer()
{
    delete dev;
    delete parser;
}

/* send command for enter programming mode */
void programmer::enter_prog_mode()
{
    unsigned char buff[64];

    memset(buff, 0, sizeof(buff));
    buff[0] = CMD_ENTER_PROG;
    buff[1] = UNKNOWN_PROG;
    buff[2] = this->dev->spi->count_crc_tx(&buff[1], sizeof(buff) - 1);

    this->dev->write_data(buff, 3);
}

void programmer::enter_endprog_mode()
{
    unsigned char buff[64];

    memset(buff, 0, sizeof(buff));
    buff[0] = CMD_FOR_CK;
    buff[1] = 0xF3;
    buff[2] = 0x83;
    buff[3] = 0xDE;
    buff[4] = 0x01;
    buff[5] = 0xFF;
    buff[6] = this->dev->spi->count_crc_tx(&buff[1], sizeof(buff) - 1);

    this->dev->write_read_data(buff, 9, 8, 1);


}


/* send command for request module id and os version */
int programmer::request_module_id(void)
{
    int ret_val = 0;
    unsigned char buff[64];

    memset(buff, 0, sizeof(buff));

    buff[0] = CMD_FOR_CK;
    buff[1] = 0xF5;
    buff[2] = 0x81;
    buff[3] = 0x00;
    buff[4] = this->dev->spi->count_crc_tx(&buff[1], sizeof(buff) -1);

    if (this->dev->write_read_data(buff, 11, 10, 1))
        ret_val = 1;

    return ret_val;
}

/*
 * retrieve module informations
 * normal command for retrieve data is used
 * but crc doesn't fit so get rid of CRC
 */
int programmer::get_module_id(void)
{
    int ret_val = 0, len, i;
    unsigned char buff[64];

    memset(buff, 0, sizeof(buff));

    buff[0] = CMD_FOR_CK;
    buff[1] = 0xF0;
    buff[2] = 0x08;
    buff[11] = this->dev->spi->count_crc_tx(&buff[1], sizeof(buff) -1);

    if ((len = this->dev->write_read_data(buff, 14, 13, 1)))
        ret_val = 1;

    printf("Module ID:");
    for (i = 0; i < len; i++) {
        printf("%02X ", buff[i+2]);
        this->module_id[i] = buff[i+2];
    }

    printf("\n");

    return ret_val;
}

/* prepare data for usb transmission */
int programmer::send_prog_data(int data_type, unsigned char *data, int data_len,
                        unsigned short addr)
{
    int i, data_fill = 0, ret_val = 0;
    unsigned char buff[64];

    memset(buff, 0, sizeof(buff));
    switch (data_type) {
    case EEPROM_USER:
        buff[0] = CMD_PROG;
        buff[1] = EEPROM_DATA;
        buff[2] = 0x83;
        /* address is just one byte */
        buff[3] = addr & 0xFF;
        buff[4] = data_len;

        memcpy(&buff[5], data, data_len);
        buff[5 + data_len] = this->dev->spi->count_crc_tx(&buff[1], sizeof(buff) -1);
        break;
    case EEPROM_APP:
        buff[0] = CMD_PROG;
        buff[1] = EEPROM_DATA;
        buff[2] = UNKNOWN;
        /* address is just one byte */
        buff[3] = addr & 0xFF;
        buff[4] = data_len;

        memcpy(&buff[5], data, data_len);
        buff[5 + data_len] = this->dev->spi->count_crc_tx(&buff[1], sizeof(buff) -1);
        break;

    case FLASH_PROG:
        buff[0] = CMD_PROG;
        buff[1] = FLASH_DATA;
        buff[2] = UNKNOWN;
        buff[3] = addr & 0xFF;
        buff[4] = (addr >> 8) & 0xFF;

        memcpy(&buff[5], data, data_len);
        /* if we have less data then block size
           fill it with 0xFF and 0x3F
         */
        if (data_len < FLASH_BLOCK_SIZE) {
            data_fill = FLASH_BLOCK_SIZE - data_len;

            for (i = 0; i < data_fill; i++) {
                buff[5 + data_len + i++] = 0xFF;
                buff[5 + data_len + i] = 0x3F;
            }
        }
        buff[5 + data_len + data_fill] = this->dev->spi->count_crc_tx(&buff[1],
                        sizeof(buff) -1);
        break;
    default:
       fprintf(stderr, "Unsupported data!\n");
        break;
    }

    if (this->dev->write_read_data(buff, BUF_LEN, BUF_LEN, 1))
            ret_val = 1;
    return ret_val;
}

/* reset module command */
int programmer::reset_module()
{
    int ret_val = 0, len;
    unsigned char buff[64];

    memset(buff, 0, sizeof(buff));

    buff[0] = CMD_ENTER_PROG;
    buff[1] = 0x8B;
    buff[2] = this->dev->spi->count_crc_tx(&buff[1], sizeof(buff) -1);;

    if ((len = this->dev->write_data(buff, 3)))
        ret_val = 1;

}
