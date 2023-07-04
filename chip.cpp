#include "chip.hh"
#include "keymap.hh"
#include <random>

Chip8 chip = {};

void initialize() {
    chip = {
        .memory     = {0},
        .SP         = 0,
        .V          = {0},
        .stack      = {0},
        .PC         = 0x200,
        .opcode     = 0,
        .index      = {0},
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
        case 0x7000:
            chip.V[chip.opcode & 0x0F00] += chip.opcode & 0x00FF;
            break;
        case 0x8000:
            switch(chip.opcode & 0x000F)
                case 0x0:       //8xy0 - LD Vx, Vy
                    chip.V[chip.opcode & 0x0F00] = chip.V[chip.opcode & 0x00F0];
                    break;
                case 0x0001:    //8xy1 - OR Vx, Vy
                    chip.V[chip.opcode & 0x0F00] |= chip.V[chip.opcode & 0x00F0];
                    break;
                case 0x0002:    //8xy2 - AND Vx, Vy
                    chip.V[chip.opcode & 0x0F00] &= chip.V[chip.opcode & 0x00F0];
                    break;
                case 0x0003:    //8xy3 - XOR Vx, Vy
                    chip.V[chip.opcode & 0x0F00] ^= chip.V[chip.opcode & 0x00F0];
                    break;
                case 0x0004:    //8xy4 - ADD Vx, Vy
                    chip.V[0xF] = chip.V[chip.opcode & 0x0F00] += chip.V[chip.opcode & 0x00F0] > 255 ? 1 : 0;
                    chip.V[chip.opcode & 0x0F00] += chip.V[chip.opcode & 0x00F0] > 255 ? 1 : 0;
                    break;
                case 0x0005:
                    auto Vy = chip.V[chip.opcode & 0x00F0];
                    auto Vx = chip.V[chip.opcode & 0x0F00] > Vy ? 1 : 0;
                    auto VF = chip.V[0xF];
                    Vx = Vx - Vy;
                    break;
                case 0x0006:
                    chip.V[0xF] = chip.V[chip.opcode & 0x0F00] & 1 ? 1 : 0;
                    chip.V[chip.opcode & 0x0F00] / 2;
                    break;
                case 0x0007:
                    auto Vx = chip.V[chip.opcode & 0x0F00];
                    auto Vy = chip.V[chip.opcode & 0x00F0] > Vx ? 1 : 0;
                    auto VF = chip.V[0xF];
                    Vx = Vy - Vx;
                    break;
                case 0x000E:
                    chip.V[0xF] = chip.V[chip.opcode & 0x0F00] << 1 ? 1 : 0;
                    break; //make sure to check this pls

            break;

    }
    //execute
}

void load_rom(){

}