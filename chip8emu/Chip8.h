#ifndef CHIP8_H
#define CHIP8_H


#include <cstdint>
#include "constants.h"
#include "roms.h"
#include <array>
#include <stdlib.h>


class Chip8 {
    public:
        Chip8();
        ~Chip8();
        void load(const unsigned char* romArray, unsigned int size);
        void emulateCycle();

        void toggleDrawFlag();
        bool getDrawFlag();
        void toggleWaitFlag();
        bool getWaitFlag();
        void updateTimers();
        void reset();
        void handleKeyPause(uint8_t keyPress);

        // Keypad
        std::array<uint8_t, MAX_SIZE> keymap;
        std::array<uint8_t, MAX_SIZE> key;

        // Screen graphics
        std::array<uint16_t, WIDTH * HEIGHT> gfx;


    private:
        void clearDisplay();

        // chip8 standard variables
        uint8_t randGen();
        std::array<uint8_t, MEMORY_SIZE> memory;
        std::array<uint8_t, MAX_SIZE> registerV;
        uint16_t I;
        uint16_t pc;
        volatile uint8_t delay_timer;
        volatile uint8_t sound_timer;
        std::array<uint16_t, MAX_SIZE> stack;
        uint8_t sp;
        std::array<uint8_t, FONT_SIZE> font;
        bool drawFlag;
        bool waitFlag;
        bool isPausedForKey;
        uint8_t targetRegX;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif