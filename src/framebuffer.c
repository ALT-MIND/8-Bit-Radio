#include "framebuffer.h"

void fb_set_pixel(uint8_t framebuffer[], uint8_t x, uint8_t y)
{
    if (x >= WIDTH || y >= HEIGHT)
    {
        // Check if the coordinates are within the screen boundaries
        return;
    }
    int y_page = y / 8;
    int byte_to_mod = x + (y_page * 128);
    int bit_to_set = 0x01 << (y & 0x07);
    framebuffer[byte_to_mod] |= bit_to_set;
}

void fb_set_image(uint8_t framebuffer[], const uint8_t image[], uint8_t offset_x, uint8_t offset_y)
{
    for (int i = 0; i < 8; i++)
    {
        framebuffer[offset_y * 16 + offset_x] = image[i];
        offset_x++;
    }
}

// Function to clear the frame buffer
void fb_clear(uint8_t framebuffer[])
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        framebuffer[i] = 0;
    }
}
