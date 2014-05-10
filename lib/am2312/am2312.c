#include "am2312.h"

#include <string.h>
#include <nrf24le1.h>
#include <nordic_common.h>
#include <hal_delay.h>
#include <hal_w2.h>

static 
uint16_t helper_crc16(unsigned char *ptr, unsigned char len) {
    uint16_t crc = 0xFFFF; 
    uint8_t  i   = 0;
    while(len--) {
        crc ^= *ptr++; 
        for(i = 0 ; i < 8 ; i++) {
            if(crc & 0x01) {
                crc >>= 1;
                crc  ^= 0xA001; 
            }
            else {
                crc >>= 1;
            } 
        }
    }
    return crc; 
}

static 
void helper_read(uint8_t* buff, uint8_t address, uint8_t firstReg, uint8_t regCount) {
    // wakeup
    hal_w2_write(address, 0, 0);

    // command
    buff[0] = PARAM_AM2321_READ;
    buff[1] = firstReg;
    buff[2] = regCount;
    hal_w2_write(address, buff, 3);

    delay_us(1800);

    // read
    hal_w2_read(address, buff, 2 + regCount + 2); // cmd + reg Array + crc
}

am2312_ACInfo am2312_read_ac() {
    enum { len = 32 };
	
    uint8_t buff[len];
	uint16_t crc;
    am2312_ACInfo info;

    memset(&buff, 0, len);
    memset(&info, 0, sizeof(am2312_ACInfo));

    helper_read(buff, I2C_ADDR_AM2321, REG_AM2321_HUMIDITY_MSB, 4);

    info.humidity     = buff[2] << 8;
    info.humidity    += buff[3];

    info.temperature  = buff[4] << 8;
    info.temperature += buff[5];

    crc  = buff[6] << 8;
    crc += buff[7];

    info.isOK = crc == helper_crc16(buff+2, 4);
    return info;
}

am2312_DeviceInfo am2312_read_device() {
    enum { len = 32 };
    
    uint8_t buff[len];
    uint16_t crc;
    am2312_DeviceInfo info;

    memset(&buff, 0, len);
    memset(&info, 0, sizeof(am2312_DeviceInfo));

    helper_read(buff, I2C_ADDR_AM2321, REG_AM2321_DEVICE_TYPE, 7);

    info.type    = buff[2] << 8;
    info.type   += buff[3];

    info.version = buff[4];

    info.id  = buff[5] << 32;
    info.id  = buff[6] << 16;
    info.id  = buff[7] << 8;
    info.id  = buff[8];


    crc  = buff[9] << 8;
    crc += buff[10];

    info.isOK = crc == helper_crc16(buff+2, 7);
    return info;
}
