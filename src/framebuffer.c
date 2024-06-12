#include "framebuffer.h"
#include "ascii.h"
#include <string.h>

void fbSetPixel(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= WIDTH || y >= HEIGHT)
    {
        // Check if the coordinates are within the screen boundaries
        return;
    }
    int yPage = y / 8;
    int byteToMod = x + (yPage * 128);
    int bitToSet = 0x01 << (y & 0x07);
    if (color)
        framebuffer[byteToMod] |= bitToSet;
    else
        framebuffer[byteToMod] &= ~bitToSet;
}

void fbText(uint8_t framebuffer[], uint8_t x, uint8_t y, char *text)
{
    for (uint8_t i = 0; i < (uint8_t)strlen(text); i++)
    {
        fbLoadImage(framebuffer, ASCII_Lookup[text[i] - 32], x + i * (FONT_WIDTH + 1), y);
    }
}

void fbINum(uint8_t framebuffer[], uint8_t x, uint8_t y, uint16_t num)
{
    char buf[5];
    char resultBuf[6];

    uint8_t i = 4;
    uint8_t j = 0;

    if (num == 0){
        fbText(framebuffer, x, y, "0");
        return;
    }

    while (num > 0){
        if (num >= 10) buf[i] = (char)(48 + num % 10);
        else{
            buf[i] = (char)(48 + num);
            break;
        };
        num /= 10;
        i--;
    }

    while (i < 5){
        resultBuf[j] = buf[i];
        j++;
        i++;
    }

    resultBuf[j] = '\0';

    fbText(framebuffer, x, y, resultBuf);
    return;

}

void fbHLine(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t width, uint8_t color)
{
    for (uint8_t i = 0; i < width; i++)
    {
        fbSetPixel(framebuffer, x + i, y, color);
    }
}

void fbVLine(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t height, uint8_t color)
{
    for (uint8_t i = 0; i < height; i++)
    {
        fbSetPixel(framebuffer, x, y + i, color);
    }
}

void fbRect(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    fbHLine(framebuffer, x, y, width, 1);
    fbHLine(framebuffer, x, y + height - 1, width, 1);
    fbVLine(framebuffer, x, y, height, 1);
    fbVLine(framebuffer, x + width - 1, y, height, 1);
}

void fbFillRect(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    for (uint8_t i = 0; i < height; i++)
    {
        fbHLine(framebuffer, x, y + i, width, color);
    }
}

void fbBox(uint8_t framebuffer[], uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    fbRect(framebuffer, x, y, width, height);
    fbFillRect(framebuffer, x + 1, y + 1, width - 2, height - 2, 0);
}

void fbLoadImage(uint8_t framebuffer[], const uint8_t image[], uint8_t offsetX, uint8_t offsetY)
{

    uint8_t imgRow;
    for (uint8_t i = 0; i < 8; i++)
    {
        imgRow = image[i];
        for (uint8_t y = offsetY; y < offsetY + 8; y++)
        {
            if ((imgRow & 0x01))
            {
                fbSetPixel(framebuffer, offsetX + i, y, 1);
            }
            imgRow = imgRow >> 1;
        }
    }
}

void fbClear(uint8_t framebuffer[])
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        framebuffer[i] = 0;
    }
}
