#include "drivers/ssd1306.h"

const uint8_t commands[] = {
    SET_DISP,     // display off
    SET_MEM_ADDR, // address setting
    0x00,         // horizontal

    // resolution and layout
    SET_DISP_START_LINE,  // start at line 0
    SET_SEG_REMAP | 0x01, // column addr 127 mapped to SEG0
    SET_MUX_RATIO,
    31,
    SET_COM_OUT_DIR | 0x08, // scan from COM[N] to COM0
    SET_DISP_OFFSET,
    0x00,
    SET_COM_PIN_CFG,
    0x02,

    // timing and driving scheme
    SET_DISP_CLK_DIV,
    0x80,
    SET_PRECHARGE,
    0xF1,
    SET_VCOM_DESEL,
    0x30, // 0.83*Vcc

    // display
    SET_CONTRAST,
    0xFF,          // maximum
    SET_ENTIRE_ON, // output follows RAM contents
    SET_NORM_INV,  // not inverted
    SET_IREF_SELECT,
    0x30, // enable internal IREF during display on
    // charge pump
    SET_CHARGE_PUMP,
    0x14,
    SET_DISP | 0x01,

};

const uint8_t prepareFBLoad[] = {
    SET_COL_ADDR,
    0,
    127,
    SET_PAGE_ADDR,
    0,
    3,
};

I2C_ERROR sendCMD(uint8_t cmd)
{
    SEND_DATE(0x80);
    while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
    {
    }
    if (FLAG_STATE(I2C_SR2, I2C_SLAVE_ACK_FAILURE))
        return DATA_NACK;
    SEND_DATE(cmd);
    while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
    {
    }
    if (FLAG_STATE(I2C_SR2, I2C_SLAVE_ACK_FAILURE))
        return DATA_NACK;
    return SUCCESS;
}

I2C_ERROR sendFramebuffer(const uint8_t data[], uint16_t length)
{
    I2C_ERROR operationStasus = SUCCESS;

    GEN_START;

    while (!checkI2CEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    SEND_7BIT_ADDRESS(SSD1306_I2C_ADDRESS, DIRECTION_TX);
    while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
        if (waitTransferComplete(DIRECTION_TX))
        {
            operationStasus = ADDRESS_NACK;
            goto Finish;
        }

    for (uint8_t i = 0; i < 6; i++)
    {
        if (sendCMD(prepareFBLoad[i]))
        {
            operationStasus = DATA_NACK;
            goto Finish;
        }
    }

    SEND_DATE(0x40);

    while (I2C->SR1.TXE == 0)
    {
    }
    if (I2C->SR2.AF == 1)
    {
        operationStasus = DATA_NACK;
        goto Finish;
    }

    for (int i = 0; i < length; ++i)
    {
        SEND_DATE(data[i]);
        while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
        {
        }
        if (FLAG_STATE(I2C_SR2, I2C_SLAVE_ACK_FAILURE))
        {
            operationStasus = DATA_NACK;
            break;
        }
    }

Finish:
    GEN_STOP;
    return operationStasus;
}

I2C_ERROR initScreen()
{
    I2C_ERROR operationStasus = SUCCESS;
    GEN_START;

    while (!checkI2CEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    SEND_7BIT_ADDRESS(SSD1306_I2C_ADDRESS, DIRECTION_TX);
    while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
        if (waitTransferComplete(DIRECTION_TX))
        {
            operationStasus = ADDRESS_NACK;
            goto Finish;
        }

    for (int i = 0; i < 27; ++i)
    {
        operationStasus = sendCMD(commands[i]);
        if (operationStasus != SUCCESS)
            break;
    }

Finish:
    GEN_STOP;
    return operationStasus;
}
