#include <stdio.h>
#include <string.h>
#include "programmer.h"

programmer::programmer()
{
    this->dev = new iqrf_dev;
}

programmer::~programmer()
{
    delete dev;
}

/* send command for enter programming mode */
void programmer::enter_prog_mode()
{
    unsigned char buff[64];

    memset(buff, 0, sizeof(buff));
    buff[0] = CMD_ENTER_PROG;
    buff[1] = UNKNOWN_PROG;
    buff[2] = this->dev->spi->count_crc_tx(&buff[1], sizeof(buff) - 1);

    this->dev->write_data(buff, 3, 3, 0);
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

    if (this->dev->write_data(buff, 11, 10, 1))
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

    if ((len = this->dev->write_data(buff, 14, 13, 1)))
        ret_val = 1;

    printf("Module ID:");
    for (i = 0; i < 4; i++) {
        printf("%02X ", buff[i+4]);
        this->module_id[i] = buff[i+4];
    }
    this->module_id[4] = '\0';
    printf("\n");

    return ret_val;
}


