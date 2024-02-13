#include "peripheral/i2c.h"

/**
 * Checks if the given I2C event has occurred.
 *
 * @param event The I2C event to check for.
 *
 * @return Returns true if the event has occurred, false otherwise.
 *
 */
uint8_t checkI2CEvent(uint16_t event)
{
    uint16_t tmp = ((uint16_t)((uint16_t)I2C->SR3.registerValue << (uint16_t)8) | (uint16_t)I2C->SR1.registerValue);
    CLEAR_SR3;
    return ((tmp & event) == event);
}

/**
 * Waits for the transfer to complete in the given data direction.
 *
 * @param dataDirection the data direction for the transfer
 *
 * @return 0 if the transfer is complete, 1 if there is a slave ACK failure
 *
 */
uint8_t waitTransferComplete(uint8_t dataDirection)
{
    uint16_t event = dataDirection ? I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED : I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED;
    while (!checkI2CEvent(event))
    {
        if (FLAG_STATE(I2C->SR2.registerValue, I2C_SLAVE_ACK_FAILURE))
        {
            CLEAR_AF_FLAG;
            return 1;
        };
    }
    return 0;
}