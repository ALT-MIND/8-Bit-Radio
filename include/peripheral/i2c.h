#ifndef INC_8_BIT_RADIO_I2C_H
#define INC_8_BIT_RADIO_I2C_H

#include "stm8.h"

typedef enum i2c_error_t{
    SUCCESS = 0,
    TRANSMISSION_ERROR,
    RECEIVING_ERROR,
    ADDRESS_NACK,
    DATA_NACK
} I2C_ERROR;

typedef struct
{
    union
    {
        struct
        {
            uint8_t PE : 1;
            const uint8_t _RESERVED : 5;
            uint8_t ENGC : 1;
            uint8_t NOSTRETCH : 1;
        };
        uint8_t registerValue;
    } CR1;

    union
    {
        struct
        {
            uint8_t START : 1;
            uint8_t STOP : 1;
            uint8_t ACK : 1;
            uint8_t POS : 1;
            const uint8_t _RESERVED : 3;
            uint8_t SWRST : 1;
        };
        uint8_t registerValue;
    } CR2;

    union
    {
        struct
        {
            uint8_t FREQ : 5;
            const uint8_t _RESERVED : 2;
        };
        uint8_t registerValue;
    } FREQR;

    union
    {
        struct
        {
            uint8_t ADD0 : 1;
            uint8_t ADD : 7;
        };
        uint8_t registerValue;
    } OARL;

    union
    {
        struct
        {
            uint8_t : 1;
            uint8_t ADD : 2;
            uint8_t : 3;
            uint8_t ADDCONF : 1;
            uint8_t ADDMODE : 1;
        };
        uint8_t registerValue;
    } OARH;

    uint8_t;
    uint8_t DR;

    union
    {
        struct
        {
            uint8_t SB : 1;
            uint8_t ADDR : 1;
            uint8_t BTF : 1;
            uint8_t ADD10 : 1;
            uint8_t STOPF : 1;
            const uint8_t _RESERVED : 1;
            uint8_t RXNE : 1;
            uint8_t TXE : 1;
        };
        uint8_t registerValue;
    } SR1;

    union
    {
        struct
        {
            uint8_t BERR : 1;
            uint8_t ARLO : 1;
            uint8_t AF : 1;
            uint8_t OVR : 1;
            const uint8_t _RESERVED_2 : 1;
            uint8_t WUFH : 1;
            const uint8_t _RESERVED : 2;
        };
        uint8_t registerValue;
    } SR2;

    union
    {
        struct
        {
            uint8_t MSL : 1;
            uint8_t BUSY : 1;
            uint8_t TRA : 1;
            const uint8_t _RESERVED_2 : 1;
            uint8_t GENCALL : 1;
            const uint8_t _RESERVED : 2;
            uint8_t DUALF : 1;
        };
        uint8_t registerValue;
    } SR3;

    union
    {
        struct
        {
            uint8_t ITERREN : 1;
            uint8_t ITEVTEN : 1;
            uint8_t ITBUFEN : 1;
            const uint8_t _RESERVED : 5;
        };
        uint8_t registerValue;
    } ITR;

    uint8_t CCRL;

    union
    {
        struct
        {
            uint8_t CCR : 4;
            const uint8_t _RESERVED : 2;
            uint8_t DUTY : 1;
            uint8_t FS : 1;
        };
        uint8_t registerValue;
    } CCRH;

    union
    {
        struct
        {
            uint8_t TRISE : 6;
            const uint8_t _RESERVED : 2;
        };
        uint8_t registerValue;
    } TRISER;
} I2C_t;

#define I2C ((volatile I2C_t *)0x5210)

#define I2C_EVENT_MASTER_MODE_SELECT (uint16_t)0x0301                 /*!< BUSY, MSL and SB flag */
#define I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED (uint16_t)0x0782   /*!< BUSY, MSL, ADDR, TXE and TRA flags */
#define I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED (uint16_t)0x0302      /*!< BUSY, MSL and ADDR flags */
#define I2C_EVENT_MASTER_MODE_ADDRESS10 (uint16_t)0x0308              /*!< EV8_2: TRA, BUSY, MSL, TXE and BTF flags */
#define I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED (uint16_t)0x0202    /*!< BUSY and ADDR flags */
#define I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED (uint16_t)0x0682 /*!< TRA, BUSY, TXE and ADDR flags */
#define I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED (uint16_t)0x1200  /*!< EV2: GENCALL and BUSY flags */
#define I2C_EVENT_SLAVE_BYTE_RECEIVED (uint16_t)0x0240             /*!< BUSY and RXNE flags */
#define I2C_EVENT_SLAVE_BYTE_TRANSMITTED (uint16_t)0x0684            /*!< TRA, BUSY, TXE and BTF flags */
#define I2C_EVENT_SLAVE_BYTE_TRANSMITTING (uint16_t)0x0680           /*!< TRA, BUSY and TXE flags */
#define I2C_EVENT_SLAVE_STOP_DETECTED (uint8_t)0x10                  /*!< STOPF flag */
#define I2C_SLAVE_ACK_FAILURE (uint8_t)0x04                           /*!< AF flag */
#define I2C_BUS_BUSY (uint8_t)0x02                                    /*!< BUSY flag */

#define BLANK_READ(x) __asm ld a, x __endasm
#define CLEAR_SR1 BLANK_READ(0x5217)
#define CLEAR_SR2 BLANK_READ(0x5218)
#define CLEAR_SR3 BLANK_READ(0x5219)
#define GEN_START __asm bset 0x5211, #0 __endasm
#define GEN_STOP __asm bset 0x5211, #1 __endasm
#define CLEAR_AF_FLAG __asm bres 0x5218, #2 __endasm
#define FLAG_STATE(register, flag) (register & flag)
#define DIRECTION_TX 0
#define DIRECTION_RX 1
#define SEND_7BIT_ADDRESS(address, direction) I2C->DR = (uint8_t)(address << 1 | direction)
#define SEND_DATE(byte) I2C->DR = byte

uint8_t checkI2CEvent(uint16_t event);
uint8_t waitTransferComplete(uint8_t dataDirection);

#endif // INC_8_BIT_RADIO_I2C_H
