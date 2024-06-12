#ifndef INC_8_BIT_RADIO_OLED_DRIVER_H
#define INC_8_BIT_RADIO_OLED_DRIVER_H

#include "peripheral/i2c.h"
#define SSD1306_I2C_ADDRESS 0x3C


#define SET_CONTRAST 0x81
#define SET_ENTIRE_ON 0xA4
#define SET_NORM_INV 0xA6
#define SET_INV 0xA7
#define SET_DISP 0xAE
#define SET_MEM_ADDR 0x20
#define SET_COL_ADDR 0x21
#define SET_PAGE_ADDR 0x22
#define SET_DISP_START_LINE 0x40
#define SET_SEG_REMAP 0xA0
#define SET_MUX_RATIO 0xA8
#define SET_IREF_SELECT 0xAD
#define SET_COM_OUT_DIR 0xC0
#define SET_DISP_OFFSET 0xD3
#define SET_COM_PIN_CFG 0xDA
#define SET_DISP_CLK_DIV 0xD5
#define SET_PRECHARGE 0xD9
#define SET_VCOM_DESEL 0xDB
#define SET_CHARGE_PUMP 0x8D

I2C_ERROR sendCMD(uint8_t cmd);
I2C_ERROR sendFramebuffer(const uint8_t data[], uint16_t length);
I2C_ERROR initScreen();   

#endif // INC_8_BIT_RADIO_OLED_DRIVER_H
