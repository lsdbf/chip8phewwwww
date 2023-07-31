#include <stdlib.h>
#include <time.h>
#include <iostream>

const unsigned int width = 64;
const unsigned int height = 32;
const unsigned int MAX_MEMORY = 0xFFF;
const unsigned int FONT_SIZE = 80;//16 sprites * 5 bytes long
const unsigned int FONT_LOAD = 0x050; //for some reason it's common to load at this address

typedef struct{
    uint8_t memory[MAX_MEMORY];
    uint8_t delay;
    uint8_t sound;
    uint8_t SP;
    uint8_t V[16];
    uint16_t stack[16];
    uint16_t PC;
    uint16_t opcode;
    uint16_t index;
    uint16_t keyboard[16];
    uint32_t graphics[width * height];
    bool     draw_flag;


}Chip8;

extern uint16_t FONT[FONT_SIZE];

void initialize();
void cycle();
void load_rom(const char* rom_game);
void draw();
void destroySDL();
#pragma once