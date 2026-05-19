#include "Chip8.h"
#include "main.h"
#include "z_displ_ST7735.h"
#include <vector>
using namespace std;

typedef struct {
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    volatile uint8_t current_state;
    uint8_t last_state;
    uint32_t last_debounce_time;
    volatile bool is_pressed;
} ctrlBtn;

static ctrlBtn ctrl_buttons[3] = {
    { BTN_RST_GPIO_Port,  BTN_RST_Pin,  0, 0, 0, false },
    { BTN_BACK_GPIO_Port, BTN_BACK_Pin, 0, 0, 0, false },
    { BTN_NEXT_GPIO_Port, BTN_NEXT_Pin, 0, 0, 0, false }
};

void handleDebounce(void);   
void drawImage(Chip8 chip8);

extern "C" {
    void main_emu(void) {

        Chip8 chip8;
        const int numRoms = 0;
        int romIndex = 0;
        array<const unsigned char *, numRoms> roms = {};
        array<const unsigned int, numRoms> romSize = {};

        chip8.load(roms[romIndex], romSize[romIndex]);

        // Command Loop
        bool running = true;
        bool reset = false;


        while (running) {
            uint8_t rightRead = (HAL_GPIO_ReadPin(BTN_RIGHT_GPIO_Port, BTN_RIGHT_Pin) == GPIO_PIN_RESET);
            uint8_t leftRead = (HAL_GPIO_ReadPin(BTN_LEFT_GPIO_Port, BTN_LEFT_Pin) == GPIO_PIN_RESET);
            uint8_t upRead = (HAL_GPIO_ReadPin(BTN_UP_GPIO_Port, BTN_UP_Pin) == GPIO_PIN_RESET);
            uint8_t downRead = (HAL_GPIO_ReadPin(BTN_DOWN_GPIO_Port, BTN_DOWN_Pin) == GPIO_PIN_RESET);

            // Keyup interrupt
            if ((chip8.key[0x9] = 1) && (rightRead == GPIO_PIN_SET)) chip8.handleKeyPause(0x9);
            if ((chip8.key[0x7] = 1) && (leftRead == GPIO_PIN_SET)) chip8.handleKeyPause(0x7);
            if ((chip8.key[0x5] = 1) && (upRead == GPIO_PIN_SET)) chip8.handleKeyPause(0x5);
            if ((chip8.key[0x8] = 1) && (downRead == GPIO_PIN_SET)) chip8.handleKeyPause(0x8);

            chip8.key[0x9] = rightRead;
            chip8.key[0x7] = leftRead;
            chip8.key[0x5] = upRead;
            chip8.key[0x8] = downRead;
            
            handleDebounce();
            if (ctrl_buttons[RST_BTN].is_pressed) {
                ctrl_buttons[RST_BTN].is_pressed = false;
                reset = true;
            } 
            else if (ctrl_buttons[BACK_BTN].is_pressed) {
                ctrl_buttons[BACK_BTN].is_pressed = false;
                romIndex = max(0, romIndex - 1);
                reset = true;
            } 
            else if (ctrl_buttons[NEXT_BTN].is_pressed) {
                ctrl_buttons[NEXT_BTN].is_pressed = false;
                romIndex = min(numRoms - 1, romIndex + 1);
                reset = true;
            }

            if (reset) {
                chip8.reset();
                chip8.load(roms[romIndex], romSize[romIndex]);
                reset = false;
                continue;
            }

            for (int i = 0; i < 11; i++) {
                chip8.emulateCycle();
                if (chip8.getDrawFlag()) { 
                    chip8.toggleWaitFlag();
                    break; 
                }
            }

            chip8.updateTimers();
            
            if (chip8.getDrawFlag()) {
                drawImage(chip8);
                chip8.toggleDrawFlag();
            }
        }
        return;

    }
}

void handleDebounce() {
    uint32_t current_time = HAL_GetTick();
    for (int i = 0; i < 3; i++) {
        uint8_t read = (HAL_GPIO_ReadPin(ctrl_buttons[i].GPIOx, ctrl_buttons[i].GPIO_Pin) == GPIO_PIN_RESET);

        if (read != ctrl_buttons[i].last_state) {
            ctrl_buttons[i].last_debounce_time = current_time;
            ctrl_buttons[i].last_state = read;
        }

        if ((current_time - ctrl_buttons[i].last_debounce_time) >= DEBOUNCE_DELAY_MS) {
            if (read != ctrl_buttons[i].current_state) {
                ctrl_buttons[i].current_state = read;
                
                if (ctrl_buttons[i].current_state == 1) {
                    ctrl_buttons[i].is_pressed = true;
                }
            }
        }
    }
}

void drawImage(Chip8 chip8) {
    static vector<uint16_t> buffer(scaleWidth * (scaleHeight));

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            uint16_t pixel = (chip8.gfx[y * WIDTH + x] != 0x0000) ? WHITE : BLACK;
            
            for (int i = 0; i < SCALE; ++i) {
                for (int j = 0; j < SCALE; ++j) {
                    int scaleX = (x * SCALE) + j;
                    int scaleY = (y * SCALE) + i;
                    if (scaleX < border || scaleX >= (scaleWidth - border) ||
                        scaleY < border || scaleY >= (scaleHeight - border)) {
                        buffer[scaleY * scaleWidth + scaleX] = D_WHITE;
                    } else {
                        buffer[scaleY * scaleWidth + scaleX] = pixel;
                    }
                }
            }
        }
    }


    Displ_DrawImage(xPos, yPos, scaleWidth, scaleHeight, reinterpret_cast<uint8_t*>(buffer.data()));
}