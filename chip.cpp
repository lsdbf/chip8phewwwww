#include "chip.hh"
#include "keymap.hh"
#include <SDL.h>

Chip8 chip = {};

void initialize() {
    chip = {
        .memory     = {0},
        .SP         = 0,
        .V          = {0},
        .stack      = {0},
        .PC         = 0x200,
        .opcode     = 0,
        .index      = 0,
        .graphics   = {0},
    };

    //load sprites into memory
    for (int i = 0; i < FONT_SIZE; ++i) {
        chip.memory[i] = KEYMAP[i];
    }
}
/*
auto* memory    = &(chip.memory);
auto* opcode    = &(chip.opcode);
auto* stack     = &(chip.stack);
auto* PC        = &(chip.PC);
*/
void cycle() {
    //fetch
    chip.opcode = chip.memory[chip.PC] << 8 | chip.memory[chip.PC + 1];
    //decode
    switch(chip.opcode & 0xF000) { //check first
        case 0x0000:
            switch(chip.opcode & 0x00FF){ //check the last two
                case 0x00E0: //clear display
                    memset(chip.graphics,0,sizeof(chip.graphics) * (width*height));
                    break;
                case 0x00EE:
                    chip.PC = chip.stack[chip.SP];
                    --chip.SP;
                    break;
            }
        case 0x1000:
            chip.PC = chip.opcode & 0x0FFF;
            break;
        case 0x2000:
            chip.SP = ++chip.SP;
            chip.PC = chip.stack[chip.SP];
            chip.PC = chip.opcode & 0x0FFF;
            break;
        case 0x3000:
            if(chip.V[chip.opcode & 0x0F00] == (chip.opcode & 0x00FF))
            {
                chip.PC += 2;
            }
            break;
        case 0x4000:
            if(chip.V[chip.opcode & 0x0F00] != (chip.opcode & 0x00FF))
            {
                chip.PC += 2;
            }
            break;
        case 0x5000:
            if(chip.V[chip.opcode & 0x0F00] == chip.V[chip.opcode & 0x00F0])
            {
                chip.PC += 2;
            }
            break;
        case 0x6000:
            chip.V[chip.opcode & 0x0F00] = chip.opcode & 0x00FF;
            break;
        case 0x9000:
            if(chip.V[chip.opcode & 0x0F00] != chip.V[chip.opcode & 0x00F0])
            {
                chip.PC += 2;
            }
            break;
        case 0xA000:
            chip.index = chip.opcode & 0x0FFF;
            break;
        case 0xB000:
            chip.PC = chip.V[0] + (chip.opcode & 0x0FFF);
            break;
        case 0xC000:
            srand(time(NULL));
            chip.V[chip.opcode & 0x0F00] = (rand() % 0x0100) & (chip.opcode & 0x00FF);
            break;
        case 0xD000:
            break;
        case 0xE000:
            switch(chip.opcode & 0x00FF)
            {
                case 0xE09E:
                    break;
                case 0xE0A1:
                    break;
            }
        case 0xF000:
            switch(chip.opcode & 0x00FF)
            {
                case 0xF007:
                    chip.V[chip.opcode & 0x0F00] = chip.delay;
                    break;
                case 0xF00A:
                    break;
                case 0xF015:
                    chip.delay = chip.V[chip.opcode & 0x0F00];
                    break;
                case 0xF018:
                    chip.sound = chip.opcode & 0x0F00;
                    break;
                case 0xF01E:
                    chip.index = chip.index + chip.V[chip.opcode & 0x0F00];
                    break;
            }
    }
    //execute
}

void load_rom(){

}