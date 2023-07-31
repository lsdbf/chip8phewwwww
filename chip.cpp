#include "chip.hh"
#include "keymap.hh"
#include <SDL.h>
#include <random>
#include <fstream>
#include <vector>

Chip8 chip = {};

void initialize() {
    chip = {
            .memory = {0},
            .SP = 0,
            .V = {0},
            .stack = {0},
            .PC = 0x200,
            .opcode = 0,
            .index = 0,
            .keyboard = {0},
            .graphics = {0},
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
    switch(chip.opcode & 0xF000){ //check first
        case 0x0000:
            switch(chip.opcode & 0x00FF){ //check the last two
                case 0x00E0: //clear display
                    memset(chip.graphics,0,sizeof(chip.graphics));
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
        case 0xD000: //display
        {
            uint8_t x = chip.V[0x0F00] % 64;
            uint8_t y = chip.V[0x00F0] % 32;
            uint8_t n = chip.V[chip.opcode & 0x000F];
            chip.V[0xF] = 0;
            for (int i = 0; i <= n; i++) {//row
                auto sprite = chip.memory[chip.index + i];
                for (int j = 0; j < 8; j++) {//column
                    auto pixel = sprite & (1 << 7 - j); //sprite pixel
                    //screen pixel
                    auto screen_pixel = chip.graphics[(x + j) + (y + i) * width];
                    if (pixel == 1 && screen_pixel == 1) {
                        screen_pixel = 0;
                        chip.V[0xF] = 1;
                        if (pixel == 1 && screen_pixel == 0) {
                            screen_pixel = true;
                        }
                        screen_pixel ^= 1;
                    }
                }
            }
            break;
        }
        case 0xE000:
            SDL_Event e;

            switch(chip.opcode & 0x00FF)
            {
                case 0x009E:
                    if(e.key.keysym.sym == KEYMAP[chip.V[chip.opcode & 0x0F00]])
                        chip.PC += 2;
                    break;
                case 0x00A1:
                    if(e.key.keysym.sym != KEYMAP[chip.V[chip.opcode & 0x0F00]])
                        chip.PC += 2;
                    break;
            }
        case 0xF000:
            switch(chip.opcode & 0x00FF)
            {
                case 0x0007:
                    chip.V[chip.opcode & 0x0F00] = chip.delay;
                    break;
                case 0x000A:

                    break;
                case 0x0015:
                    chip.delay = chip.V[chip.opcode & 0x0F00];
                    break;
                case 0x0018:
                    chip.sound = chip.opcode & 0x0F00;
                    break;
                case 0x001E:
                    chip.index = chip.index + chip.V[chip.opcode & 0x0F00];
                    break;
                case 0x0029:
                    chip.index = chip.V[chip.opcode & 0x0F00];
                    break;
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
                case 0x0005: {
                    auto Vy = chip.V[chip.opcode & 0x00F0];
                    auto Vx = chip.V[chip.opcode & 0x0F00] > Vy ? 1 : 0;
                    auto VF = chip.V[0xF];
                    Vx = Vx - Vy;
                    break;
                }
                case 0x0006:
                    chip.V[0xF] = chip.V[chip.opcode & 0x0F00] & 1 ? 1 : 0;
                    chip.V[chip.opcode & 0x0F00] / 2;
                    break;
                case 0x0007:
                {
                    auto Vx = chip.V[chip.opcode & 0x0F00];
                    auto Vy = chip.V[chip.opcode & 0x00F0] > Vx ? 1 : 0;
                    auto VF = chip.V[0xF];
                    Vx = Vy - Vx;
                    break;
                }
                case 0x000E:
                    chip.V[0xF] = chip.V[chip.opcode & 0x0F00] << 1 ? 1 : 0;
                    break; //make sure to check this pls
    }
    //execute
}

void load_rom(char* rom){

    std::FILE *rom_game;

    rom_game = fopen(rom, "rb");

    if (rom_game == NULL){
        std::cerr << "ROM could not be loaded" << std::endl;
    }

    fread(&chip.memory[0x200], 1,MAX_MEMORY, rom_game);

    fclose(rom_game);

}