#include "stm8.h"

typedef enum btn_state_t
{
    NONE,
    DOWN,
    LEFT,
    RIGHT,
    CENTER,
    UP,
    SET,
    RESET
} BTN_STATE;


typedef struct{
    uint8_t isPressed : 1;
    uint8_t isReleased : 1;
    uint8_t timePressed : 3;
    uint8_t timeReleased : 3;
}btn_interaction_state_t;