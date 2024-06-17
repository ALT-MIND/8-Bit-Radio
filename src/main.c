#include "drivers/rda5807m.h"
#include "drivers/ssd1306.h"
#include "framebuffer.h"
#include "peripheral/i2c.h"
#include "stm8.h"
#include "util.h"

// #define DEBUG_UI
#define MANUAL_CONTROL

#define UPPER_CHAN_LIMIT 0xD2
#define LOWER_CHAN_LIMIT 0x00

#define ERROR_LED_PORT PA
#define ERROR_LED_PIN PIN2
#define SCREEN_STARTUP_INIT_FAILED_DELAY_HIGH 0x03
#define SCREEN_STARTUP_INIT_FAILED_DELAY_LOW 0x42

#define CONTROL_BUTTONS_PORT PC
#define CB_UP_PIN PIN3
#define CB_DOWN_PIN PIN4
#define CB_LEFT_PIN PIN5
#define CB_RIGHT_PIN PIN6
#define CB_CENTER_PIN PIN7

#define SEEK_BUTTON_PORT PD
#define SEEK_UP_BUTTON_PIN PIN1
#define SEEK_DOWN_BUTTON_PIN PIN2

const uint8_t VOLUME_ICON[] = {0x3c, 0x7e, 0xff, 0x00, 0x3c, 0x81, 0x7e, 0x00};

volatile BTN_STATE btnState;
volatile btn_interaction_state_t btnInteractionState[7];

volatile rda5807m_word reg02Mirror;
volatile rda5807m_word reg03Mirror;
volatile rda5807m_word reg05Mirror;
volatile rda5807m_word reg0AMirror;

uint8_t mutedVolume = 0;

uint8_t fb[BUFFER_SIZE];

void ErrorBlinkLed() __interrupt(TIM2_OVR_UIF_IRQ)
{
    PORT(ERROR_LED_PORT, ODR) ^= ERROR_LED_PIN;
    TIM2_SR1 &= ~0x01;
}

BTN_STATE getBtnState()
{
    for (int i = 0; i < 7; i++)
    {
        if (btnInteractionState[i].isReleased)
        {
            return (BTN_STATE)(i + 1);
        }
    }
    return NONE;
}

void ButtonCheckJob() __interrupt(TIM4_OVR_UIF_IRQ)
{
    static uint8_t counter = 0;

    if (counter == 5)
    {
        counter = 0;
        for (uint8_t i = 3; i < 8; i++)
        {
            if (!(PORT(CONTROL_BUTTONS_PORT, IDR) & (1 << i)))
            {
                if (btnInteractionState[i - 3].isPressed)
                {
                    continue;
                }
                else if (btnInteractionState[i - 3].timePressed < 5)
                {
                    btnInteractionState[i - 3].timePressed++;
                }
                else
                {
                    btnInteractionState[i - 3].isPressed = 1;
                    btnInteractionState[i - 3].timePressed = 0;
                }
            }
            else
            {
                if (btnInteractionState[i - 3].isPressed)
                {
                    btnInteractionState[i - 3].isPressed = 0;
                    btnInteractionState[i - 3].timePressed = 0;
                    btnInteractionState[i - 3].isReleased = 1;
                    btnInteractionState[i - 3].timeReleased = 0;
                }
                else if (btnInteractionState[i - 3].isReleased)
                {
                    if (btnInteractionState[i - 3].timeReleased < 5)
                    {
                        btnInteractionState[i - 3].timeReleased++;
                    }
                    else
                    {
                        btnInteractionState[i - 3].isReleased = 0;
                        btnInteractionState[i - 3].timeReleased = 0;
                    }
                }
            }
        }
#ifndef MANUAL_CONTROL        
        for (uint8_t i = 1; i < 3; i++)
        {
            if (!(PORT(SEEK_BUTTON_PORT, IDR) & (1 << i)))
            {
                if (btnInteractionState[i + 4].isPressed)
                {
                    continue;
                }
                else if (btnInteractionState[i + 4].timePressed < 5)
                {
                    btnInteractionState[i + 4].timePressed++;
                }
                else
                {
                    btnInteractionState[i + 4].isPressed = 1;
                    btnInteractionState[i + 4].timePressed = 0;
                }
            }
            else
            {
                if (btnInteractionState[i + 4].isPressed)
                {
                    btnInteractionState[i + 4].isPressed = 0;
                    btnInteractionState[i + 4].timePressed = 0;
                    btnInteractionState[i + 4].isReleased = 1;
                    btnInteractionState[i + 4].timeReleased = 0;
                }
                else if (btnInteractionState[i + 4].isReleased)
                {
                    if (btnInteractionState[i + 4].timeReleased < 5)
                    {
                        btnInteractionState[i + 4].timeReleased++;
                    }
                    else
                    {
                        btnInteractionState[i + 4].isReleased = 0;
                        btnInteractionState[i + 4].timeReleased = 0;
                    }
                }
            }
        }
#endif
    }
    counter++;
    TIM4_SR = 0;
}

I2C_ERROR checkHealth(uint8_t adrress)
{

    I2C_ERROR operationStasus = SUCCESS;

    GEN_START;

    while (!checkI2CEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;

    SEND_7BIT_ADDRESS(adrress, DIRECTION_TX);
    while ((FLAG_STATE(I2C_SR1, 0x80) == 0))
        if (waitTransferComplete(DIRECTION_TX))
        {
            operationStasus = ADDRESS_NACK;
            break;
        }

    GEN_STOP;

    return operationStasus;
}

int main(void)
{
    disableInterrupts();

    /***************************************
     *                                      *
     *        CONFIGURATION AREA            *
     *                                      *
     ***************************************/
    CLK_CKDIVR = 0;
    CLK_PCKENR1 = 0x31; // Enable  Tim2, Tim4, I2C
    CLK_PCKENR2 = 0xF3; // All disabled, except reserved

    TIM2_PSCR = 0xE;
    TIM2_CR1 = 0x84;
    TIM2_IER = 0x01;
    TIM2_ARRH = SCREEN_STARTUP_INIT_FAILED_DELAY_HIGH;
    TIM2_ARRL = SCREEN_STARTUP_INIT_FAILED_DELAY_LOW;

    TIM4_PSCR = 7;
    TIM4_ARR = 0xFF;
    TIM4_IER = 1;
    TIM4_CNTR = 0x00;
    TIM4_CR1 = 0x01;

    I2C->FREQR.FREQ = 0x10;
    I2C->CR1.PE = 0;
    I2C->TRISER.TRISE = 0x11;
    I2C->CCRL = 80;
    I2C->OARH.ADDCONF = 1;
    I2C->CR1.PE = 1;
    I2C->CR2.ACK = 1;

    PORT(ERROR_LED_PORT, CR1) |= ERROR_LED_PIN;
    PORT(ERROR_LED_PORT, DDR) |= ERROR_LED_PIN;
    PORT(ERROR_LED_PORT, CR2) |= ERROR_LED_PIN;

    PORT(CONTROL_BUTTONS_PORT, CR1) |= CB_UP_PIN | CB_DOWN_PIN | CB_LEFT_PIN | CB_RIGHT_PIN | CB_CENTER_PIN;
    PORT(SEEK_BUTTON_PORT, CR1) |= SEEK_UP_BUTTON_PIN | SEEK_DOWN_BUTTON_PIN;

    enableInterrupts();

    fbClear(fb);

    reg02Mirror.raw = RDA_DEFAULT_REG02;
    reg03Mirror.raw = RDA_DEFAULT_REG03;
    reg05Mirror.raw = RDA_DEFAULT_REG05;

    // TODO: INIT peripherals

    if (checkHealth(SSD1306_I2C_ADDRESS) == ADDRESS_NACK)
        goto ScreenError;

    if (initScreen() != SUCCESS)
        goto ScreenError;

#ifndef DEBUG_UI
    if (checkHealth(RDA5807M_I2C_ADDRESS) == ADDRESS_NACK)
        goto RDAError;

    if (initRDA5807M(&reg02Mirror) != SUCCESS)
        goto RDAError;

    reg03Mirror.raw |= RDA_TUNE_ON;
    if (writeRDARegister(REG03, &reg03Mirror) != SUCCESS)
        goto RDAError;

#endif

    fbLoadImage(fb, VOLUME_ICON, 101, 0);
    uint8_t chan = (reg03Mirror.raw & ~RDA_CHAN_CLEAR) >> 6;
    fbINum(fb, 110, 0, (reg05Mirror.raw & (~RDA_VOLUME_CLEAR)));
    uint8_t intPart = chan / 10 + 87;
    uint8_t fracPart = chan % 10;
    fbINum(fb, 40, 19, intPart);
    fbText(fb, intPart < 100 ? 51 : 57, 19, ".");
    fbINum(fb, intPart < 100 ? 54 : 61, 19, fracPart);
    fbText(fb, intPart < 100 ? 64 : 72, 19, "FM");
    fbText(fb, 10, 19, "<");
    fbText(fb, 111, 19, ">");
    sendFramebuffer(fb, BUFFER_SIZE);

    while (1)
    {
        btnState = getBtnState();

        switch (btnState)
        {
        case UP:
        {
            uint8_t volume = reg05Mirror.raw & ~RDA_VOLUME_CLEAR;
            if (volume < 15)
            {
                volume++;
                SET_VOLUME(reg05Mirror, volume);
#ifndef DEBUG_UI
                if (writeRDARegister(REG05, &reg05Mirror) != SUCCESS)
                    goto RDAError;
#endif
                fbFillRect(fb, 110, 0, 11, 8, 0);
                fbINum(fb, 110, 0, volume);
            }
            btnInteractionState[UP - 1].isReleased = 0;
            btnInteractionState[UP - 1].timeReleased = 0;
            if (sendFramebuffer(fb, BUFFER_SIZE) != SUCCESS)
                goto ScreenError;
            break;
        }
        case DOWN:
        {
            uint8_t volume = reg05Mirror.raw & ~RDA_VOLUME_CLEAR;
            if (volume > 0)
            {
                volume--;
                SET_VOLUME(reg05Mirror, volume);
#ifndef DEBUG_UI
                if (writeRDARegister(REG05, &reg05Mirror) != SUCCESS)
                    goto RDAError;
#endif
                fbFillRect(fb, 110, 0, 11, 8, 0);
                fbINum(fb, 110, 0, volume);
            }

            btnInteractionState[DOWN - 1].isReleased = 0;
            btnInteractionState[DOWN - 1].timeReleased = 0;

            if (sendFramebuffer(fb, BUFFER_SIZE) != SUCCESS)
                goto ScreenError;

            break;
        }
        case LEFT:
        { // frequency down
            uint8_t currentCHAN = (reg03Mirror.raw & ~RDA_CHAN_CLEAR) >> 6;
            // readRDARegister(REG0A, &reg0AMirror);
            // GET_READCHAN(reg0AMirror, currenCHAN);
            if (currentCHAN > LOWER_CHAN_LIMIT)
            {
                currentCHAN--;
                reg03Mirror.raw &= RDA_CHAN_CLEAR;
                reg03Mirror.raw |= (uint16_t)currentCHAN << 6;
                reg03Mirror.raw |= RDA_TUNE_ON;
#ifndef DEBUG_UI
                if (writeRDARegister(REG03, &reg03Mirror) != SUCCESS)
                    goto RDAError;
#endif

                uint8_t intPart = currentCHAN / 10 + 87;
                uint8_t fracPart = currentCHAN % 10;
                fbFillRect(fb, 40, 19, 50, 8, 0);
                fbINum(fb, 40, 19, intPart);
                fbText(fb, intPart < 100 ? 51 : 57, 19, ".");
                fbINum(fb, intPart < 100 ? 54 : 61, 19, fracPart);
                fbText(fb, intPart < 100 ? 64 : 72, 19, "FM");
                sendFramebuffer(fb, BUFFER_SIZE);
            }
            btnInteractionState[LEFT - 1].isReleased = 0;
            btnInteractionState[LEFT - 1].timeReleased = 0;

            break;
        }
        case RIGHT:
        { // frequency up

            uint8_t currentCHAN = (reg03Mirror.raw & ~RDA_CHAN_CLEAR) >> 6;
            if (currentCHAN < UPPER_CHAN_LIMIT)
            {
                currentCHAN++;
                reg03Mirror.raw &= RDA_CHAN_CLEAR;
                reg03Mirror.raw |= (uint16_t)currentCHAN << 6;
                reg03Mirror.raw |= RDA_TUNE_ON;

#ifndef DEBUG_UI
                if (writeRDARegister(REG03, &reg03Mirror) != SUCCESS)
                    goto RDAError;
#endif

                uint8_t intPart = currentCHAN / 10 + 87;
                uint8_t fracPart = currentCHAN % 10;
                fbFillRect(fb, 40, 19, 50, 8, 0);
                fbINum(fb, 40, 19, intPart);
                fbText(fb, intPart < 100 ? 51 : 57, 19, ".");
                fbINum(fb, intPart < 100 ? 54 : 61, 19, fracPart);
                fbText(fb, intPart < 100 ? 64 : 72, 19, "FM");
                // TODO: wrap function call to error handling
                sendFramebuffer(fb, BUFFER_SIZE);
            }

            btnInteractionState[RIGHT - 1].isReleased = 0;
            btnInteractionState[RIGHT - 1].timeReleased = 0;

            break;
        }
        case CENTER:
        {
            reg02Mirror.raw ^= RDA_MUTE;
            if (reg02Mirror.raw & RDA_MUTE)
            {
#ifndef DEBUG_UI
                if (writeRDARegister(REG02, &reg02Mirror) != SUCCESS)
                    goto RDAError;
#endif
                fbFillRect(fb, 109, 0, 8, 8, 0);
                fbINum(fb, 110, 0, mutedVolume);
            }
            else
            {
                mutedVolume = (uint8_t)(reg05Mirror.raw & ~RDA_VOLUME_CLEAR);
#ifndef DEBUG_UI
                if (writeRDARegister(REG02, &reg02Mirror) != SUCCESS)
                    goto RDAError;
#endif
                fbFillRect(fb, 110, 0, 11, 8, 0);
                fbText(fb, 109, 0, "X");
            }

            btnInteractionState[CENTER - 1].isReleased = 0;
            btnInteractionState[CENTER - 1].timeReleased = 0;

            if (sendFramebuffer(fb, BUFFER_SIZE) != SUCCESS)
                goto ScreenError;

            break;
        }
        case SET:
        { // seek down
        }
        case RESET:
        {
            // seek up
            break;
        }

        }
    };

RDAError:
    fbText(fb, 7, 16, "Radiomodule broken");
    if (sendFramebuffer(fb, BUFFER_SIZE) != SUCCESS)
        goto EverythingBroken;
    for (;;)
        ;

ScreenError:
    TIM2_CR1 |= 0x01;
    for (;;)
        ;

EverythingBroken:
    PORT(ERROR_LED_PORT, ODR) |= ERROR_LED_PIN;
    for (;;)
        ;
}