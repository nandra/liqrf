#include <stdio.h>
#include <string.h>
#include "programmer.h"

//#define DEBUG_PROGRAMMER

#ifdef DEBUG_PROGRAMMER
#define DBG(format, arg...) \
    DBG("prog:" format, ##arg);
#else
#define DBG(format, arg...) {}
#endif

programmer::programmer()
{
    this->dev = new iqrf_dev;
    this->parser = new hex_parser;
    this->usr_eeprom = 0;
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

    DBG("Module ID:");
    for (i = 0; i < len; i++) {
        DBG("%02X ", buff[i+2]);
        this->module_id[i] = buff[i+2];
    }

    DBG("\n");

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
        this->usr_eeprom = 1;
        break;
    case EEPROM_APP:
        buff[0] = CMD_PROG;
        buff[1] = EEPROM_DATA;
        if (this->usr_eeprom) {
            if (data_len < 32)
                buff[2] = 0x9D;
            else
                buff[2] = 0xA2;
            this->usr_eeprom = 0;
        } else {
                buff[2] = UNKNOWN_DATA;
        }

        /* address is just one byte */
        buff[3] = addr & 0xFF;
        buff[4] = data_len;

        memcpy(&buff[5], data, data_len);
        buff[5 + data_len] = this->dev->spi->count_crc_tx(&buff[1], sizeof(buff) -1);
        break;

    case FLASH_PROG:
        buff[0] = CMD_PROG;
        buff[1] = FLASH_DATA;
        buff[2] = UNKNOWN_DATA;
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

    return ret_val;
}

/* release entries in programmer */
void programmer::release()
{
    this->dev->usb->release_usb();
}

/* send command for write/read SPI data
 * it uis used when user send response
 * for spi command
 */
int programmer::write_read_spi_data(unsigned char *data, int data_len, bool write_read)
{
    int ret_val = 0;

    if (data_len > SPI_DATA_LENGTH)
        data_len = SPI_DATA_LENGTH;

    ret_val = this->dev->get_spi_cmd_data(data, data_len, write_read);

    return ret_val;
}

/* send command for write/read SPI data
 * all bytes are defined by user
 */
int programmer::write_read_test_spi_data(unsigned char *data, int data_len)
{
    int ret_val = 0;
    unsigned char buff[64];

    buff[0] = CMD_FOR_CK;
    /* spi data len = 35 + 0xF0 + CRC */
    if (data_len > SPI_DATA_LENGTH + 2)
        data_len = SPI_DATA_LENGTH + 2;

    memcpy(&buff[1], data, data_len);
    int i;
    for (i=0; i < data_len; i++)
        DBG("[%x]", buff[i]);

    ret_val = this->dev->write_read_data(buff, data_len+1, data_len + 1, 1);
    if (ret_val)
        memcpy(data, buff, ret_val);
    return ret_val;
}

int programmer::init()
{
    return this->dev->init_device();
}

int programmer::get_status()
{
    return this->dev->get_spi_status();
}

void programmer::reset()
{
    this->dev->reset_device();
}

int programmer::crc(unsigned char *buff, int len)
{
    return this->dev->count_crc(buff, len);
}
