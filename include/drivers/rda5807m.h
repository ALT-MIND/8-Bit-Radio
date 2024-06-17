#ifndef INC_8_BIT_RADIO_RDA5807M_H
#define INC_8_BIT_RADIO_RDA5807M_H

#include "stm8.h"
#include "peripheral/i2c.h"

#define RDA5807M_I2C_ADDRESS 0x11

#define REG00 0x00 //!< Register 0x00
#define REG02 0x02 //!< Register 0x02
#define REG03 0x03 //!< Register 0x03
#define REG04 0x04 //!< Register 0x04
#define REG05 0x05 //!< Register 0x05
#define REG06 0x06 //!< Register 0x06
#define REG07 0x07 //!< Register 0x07
#define REG08 0x08 //!< Register 0x08
#define REG0A 0x0A //!< Register 0x0A
#define REG0B 0x0B //!< Register 0x0B
#define REG0C 0x0C //!< Register 0x0C
#define REG0D 0x0D //!< Register 0x0D
#define REG0E 0x0E //!< Register 0x0E
#define REG0F 0x0F //!< Register 0x0F

/* Register 0x02*/
#define RDA_DHIZ 0x8000

#define RDA_MUTE 0x4000

#define RDA_MONO_ON 0x2000
#define RDA_MONO_OFF 0xDFFF

#define RDA_BASS_ON 0x1000
#define RDA_BASS_OFF 0xEFFF

#define RDA_RCLK_MODE 0x0800
#define RDA_RCLK_DIRECT 0x0400

#define RDA_SEEK_UP 0x0200
#define RDA_SEEK_DOWN 0xFDFF
#define RDA_SEEK 0x0100

#define RDA_SEEK_WRAP 0x0080
#define RDA_SEEK_NOWRAP 0xFF7F

#define RDA_CLK_0327 0x0000
#define RDA_CLK_1200 0x0010
#define RDA_CLK_2400 0x0050
#define RDA_CLK_1300 0x0020
#define RDA_CLK_2600 0x0060
#define RDA_CLK_1920 0x0030
#define RDA_CLK_3840 0x0070

#define RDA_RDS_ON 0x0008
#define RDA_RDS_OFF 0xFFF7

#define RDA_NEW 0x0004

#define RDA_RESET 0x0002

#define RDA_POWER 0x0001

/* Register 0x03 */
#define RDA_CHAN_CLEAR 0x003f
#define RDA_TUNE_ON 0x0010
#define RDA_TUNE_OFF 0xFFEF

/* Register 0x0A */

#define RDA_STC_FLAG 0x4000
#define RDA_SF_FLAG 0x2000

/*` Register 0x05 */
#define RDA_VOLUME_CLEAR 0xfff0


// Default value for registers
// Register 0x02
#define RDA_DEFAULT_REG02 0x0000

// Register 0x03
#define RDA_DEFAULT_REG03 0x1C80 // 98.4 MHz

// Register 0x05
#define RDA_DEFAULT_REG05 0x888B


// Macros for checking some flags in 0A register
// and getting READCHAN value

#define CHECK_SEEK_TUNE_STATUS(regValue) ((regValue.raw & 0x4000) >> 14)
#define CHECK_SEEK_STATUS(regValue) ((regValue.raw & 0x2000) >> 13)
#define GET_READCHAN(regValue, resultVar) resultVar = regValue.raw & 0x3FF
#define SET_VOLUME(regValue, value) regValue.raw = (regValue.raw & RDA_VOLUME_CLEAR) | value 

typedef union
{
    struct
    {
        uint8_t hByte;
        uint8_t lByte;
    };
    uint16_t raw;
} rda5807m_word;


I2C_ERROR writeRDARegister(uint8_t regAddress, rda5807m_word *regValue);

I2C_ERROR readRDARegister(uint8_t regAddress, rda5807m_word *regValue);

I2C_ERROR initRDA5807M(rda5807m_word* initReg);

#endif