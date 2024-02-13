#include "stm8.h"

#define WIDTH  128
#define HEIGHT 32

#define BYTES_PER_ROW ((WIDTH + 7) / 8)
#define BUFFER_SIZE (BYTES_PER_ROW * HEIGHT)


void fb_set_pixel(uint8_t framebuffer[], uint8_t x, uint8_t y);
void fb_set_image(uint8_t framebuffer[], const uint8_t image[], uint8_t offset_x, uint8_t offset_y);
void fb_clear(uint8_t framebuffer[]);
