#ifndef INC_8_BIT_RADIO_FRAMEBUFFER_H
#define INC_8_BIT_RADIO_FRAMEBUFFER_H

#include "stm8.h"

#define WIDTH 128
#define HEIGHT 32

#define BUFFER_SIZE 512

void fbClear(uint8_t framebuffer[]);
void fbSetPixel(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t color);
void fbText(uint8_t framebuffer[], uint8_t x, uint8_t y, char *text);
void fbHLine(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t width, uint8_t color);
void fbVLine(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t height, uint8_t color);
void fbRect(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void fbLoadImage(uint8_t framebuffer[], const uint8_t image[], uint8_t offsetX, uint8_t offsetY);
void fbFillRect(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void fbBox(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t width, uint8_t height);

#endif // INC_8_BIT_RADIO_FRAMEBUFFER_H