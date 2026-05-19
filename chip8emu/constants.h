#ifndef CONSTANTS_H
#define CONSTANTS_H

const int WIDTH = 64;
const int HEIGHT = 32;
const int SCALE = 2;
const int MAX_SIZE = 16;
const int MEMORY_SIZE = 4096;
const int ROM_SIZE = 512;
const int FONT_SIZE = 80;
const unsigned int FONT_START_ADDRESS = 0x50;

const int SCREEN_HEIGHT = 160;
const int scaleWidth = WIDTH * SCALE;
const int scaleHeight = HEIGHT * SCALE;
const int border = 1;
const uint16_t xPos = 0;
const uint16_t yPos = (SCREEN_HEIGHT - scaleHeight) / 2;

#define DEBOUNCE_DELAY_MS 10
#define RST_BTN 0
#define BACK_BTN 1
#define NEXT_BTN 2

#endif