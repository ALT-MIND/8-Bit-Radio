#include "drivers/rda5807m.h"
#include "peripheral/i2c.h"
#include "stdio.h"

// #define DELAY_MUL_16 500

// void Delay_ms(uint32_t value)
// {
//     uint32_t toWait = DELAY_MUL_16 * value;
//     volatile uint32_t loopCounter = 0;
//     for (loopCounter; loopCounter < toWait; loopCounter++)
//     {
//     }
// }

I2C_ERROR awaitDataSend()
{
    while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
    {
    }
    if (FLAG_STATE(I2C_SR2, I2C_SLAVE_ACK_FAILURE))
    {
        return DATA_NACK;
    }
    return SUCCESS;
}

I2C_ERROR writeRDARegister(uint8_t regAddress, rda5807m_word *regValue)
{
    I2C_ERROR operationStasus = SUCCESS;

    GEN_START;

    while (!checkI2CEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    SEND_7BIT_ADDRESS(0x11, DIRECTION_TX);
    while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
        if (waitTransferComplete(DIRECTION_TX))
        {
            operationStasus = ADDRESS_NACK;
            goto Finish;
        }

    
    SEND_DATE(regAddress);
    operationStasus = awaitDataSend();
    if (operationStasus != SUCCESS)
        goto Finish;

    SEND_DATE(regValue->hByte);

    operationStasus = awaitDataSend();
    if (operationStasus != SUCCESS)
        goto Finish;


    SEND_DATE(regValue->lByte);
    operationStasus = awaitDataSend();

Finish:
    GEN_STOP;
    return operationStasus;
}

I2C_ERROR readRDARegister(uint8_t regAddress, rda5807m_word *regValue)
{
    I2C_ERROR operationStasus = SUCCESS;

    GEN_START;

    while (!checkI2CEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    SEND_7BIT_ADDRESS(0x11, DIRECTION_TX);
    while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
        if (waitTransferComplete(DIRECTION_TX))
        {
            operationStasus = ADDRESS_NACK;
            goto Finish;
        }

    SEND_DATE(regAddress);
    operationStasus = awaitDataSend();
    if (operationStasus != SUCCESS)
        goto Finish;

    GEN_STOP;

    I2C->CR2.ACK = 1;
    I2C->CR2.POS = 1;

    GEN_START;

    while (!checkI2CEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    SEND_7BIT_ADDRESS(0x11, DIRECTION_RX);
    if (waitTransferComplete(DIRECTION_RX))
    {
        operationStasus = ADDRESS_NACK;
        goto Finish;
    }

    I2C->CR2.ACK = 0;

    while (FLAG_STATE(I2C_SR1, 0x04) == 0)
    {
    }

    regValue->hByte = I2C->DR;
    regValue->lByte = I2C->DR;

Finish:
    GEN_STOP;
    return operationStasus;
}

I2C_ERROR initRDA5807M(rda5807m_word* initReg)
{

    I2C_ERROR operationStasus = SUCCESS;

    initReg->raw = RDA_DEFAULT_REG02;
    initReg->raw |= RDA_DHIZ;
    initReg->raw |= RDA_MONO_ON;
    initReg->raw |= RDA_POWER;

    operationStasus = writeRDARegister(REG02, initReg);

    return operationStasus;

}
