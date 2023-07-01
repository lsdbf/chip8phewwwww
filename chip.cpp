#include "chip.hh"
#include "keymap.hh"

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

    break;    
    }
    //execute
}

void load_rom(){

}