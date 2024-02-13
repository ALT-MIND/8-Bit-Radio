#include <string.h>
#include "stm8.h"
#include "peripheral/i2c.h"
#include "drivers/ssd1306.h"
#include "framebuffer.h"

#define DELAY_MUL_16 500

// array size is 8
const uint8_t smile[] =
    {
 0x00, 
  0x20, 
  0x40, 
  0x41, 
  0x41, 
  0x3f, 
  0x01, 
  0x00};

void Delay_ms(uint32_t value)
{
    uint32_t toWait = DELAY_MUL_16 * value;
    volatile uint32_t loopCounter = 0;
    for (loopCounter; loopCounter < toWait; loopCounter++)
    {
    }
}

void uart_write(const char *str)
{
    for (uint8_t i = 0; i < (uint8_t)strlen(str); i++)
    {
        while (!(UART1_SR & UART_SR_TXE))
            ; // !Transmit data register empty
        UART1_DR = str[i];
    } // Bytes sent
}

int main(void)
{
    /* Set clock to full speed (16 Mhz) */
    CLK_CKDIVR = 0;
    I2C->FREQR.FREQ = 0x10;
    I2C->CR1.PE = 0;
    I2C->TRISER.TRISE = 0x11;
    I2C->CCRL = 80;
    I2C->OARH.ADDCONF = 1;
    I2C->CR1.PE = 1;
    I2C->CR2.ACK = 1;

    uint8_t fb[BUFFER_SIZE];
    fb_clear(fb);
    // Setup UART1 (TX=D5)
    UART1_CR2 |= UART_CR2_TEN;                       // Transmitter enable
                                                     //    UART1_CR2 |= UART_CR2_REN; // Receiver enable
    UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2); // 1 stop bit
    // 9600 baud: UART_DIV = 16000000/9600 ~ 1667 = 0x0683
    UART1_BRR2 = 0x03;
    UART1_BRR1 = 0x68; // 0x0683 coded funky way (see ref manual)

    uart_write("Start\n\r");
    while (FLAG_STATE(I2C_SR3, I2C_BUS_BUSY))
        ;
    GEN_START;
    while (!checkI2CEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    SEND_7BIT_ADDRESS(60, DIRECTION_TX);

    if (waitTransferComplete(DIRECTION_TX))
        goto Err;

    if (initScreen())
        goto Err;

    fb_set_image(fb, smile, 64, 16);
    SEND_DATE(0x40);
    while (I2C->SR1.TXE == 0)
    {
    }
    if (I2C->SR2.AF == 1)
        goto Err;

    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        SEND_DATE(fb[i]);
        while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
        {
        }
        if (FLAG_STATE(I2C_SR2, I2C_SLAVE_ACK_FAILURE))
            goto Err;
    }

    GEN_STOP;
    uart_write("Done\n\r");
    for (;;)
    {
    }

Err:
    uart_write("Error");
    for (;;)
        ;
}