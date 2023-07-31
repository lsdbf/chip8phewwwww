#include "chip.hh"
#include "keymap.hh"
#include <SDL2/SDL.h>
#include <random>
#include <fstream>
#include <vector>

Chip8 chip = {};

SDL_Window *screen = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, 
  SDL_WINDOWPOS_CENTERED, 1200, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  | SDL_WINDOW_SHOWN);

SDL_Renderer *render = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);

SDL_Texture *texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,width,height);

uint16_t KEYS[FONT_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void initialize()
{
    chip = {
        .memory = {0},
        .delay = 0,
        .sound = 0,
        .SP = 0,
        .V = {0},
        .stack = {0},
        .PC = 0x200,
        .opcode = 0,
        .index = 0,
        .keyboard = {0},
        .graphics = {0},
        .draw_flag = false,
    };
    memset(chip.memory, 0, sizeof(chip.memory));
    memset(chip.graphics, 0, sizeof(chip.memory));
    memset(chip.V, 0, sizeof(chip.V));
    memset(chip.stack, 0, sizeof(chip.stack));
    memset(chip.keyboard, 0, sizeof(chip.keyboard));
    
    // load sprites into memory
    for (unsigned int i = 0; i < FONT_SIZE; ++i)
    {
        chip.memory[i] = KEYMAP[i];
    }
}


/*
auto* memory    = &(chip.memory);
auto* opcode    = &(chip.opcode);
auto* stack     = &(chip.stack);
auto* PC        = &(chip.PC);
*/
void cycle()
{
    // fetch
    chip.opcode = chip.memory[chip.PC] << 8 | chip.memory[chip.PC + 1];
    // decode
    switch (chip.opcode & 0xF000)
    { // check first
    case 0x0000:
        switch (chip.opcode & 0x00FF)
        {            // check the last two
        case 0x00E0: // clear display
            memset(chip.graphics, 0, sizeof(chip.graphics));
            chip.PC += 2;
            chip.draw_flag = true;
            break;
        case 0x00EE:
            --chip.SP;
            chip.PC = chip.stack[chip.SP];
            chip.PC += 2;
            break;
        }
        break;
    case 0x1000:
        chip.PC = chip.opcode & 0x0FFF;
        //chip.PC += 2;
        break;
    case 0x2000:
        ++chip.SP;
        chip.PC = chip.stack[chip.SP];
        chip.PC = chip.opcode & 0x0FFF;
        break;
    case 0x3000:
        if (chip.V[chip.opcode & 0x0F00] == (chip.opcode & 0x00FF))
        {
            chip.PC += 4;
        }
        else{
            chip.PC += 2;
        }
        break;
    case 0x4000:
        if (chip.V[chip.opcode & 0x0F00] != (chip.opcode & 0x00FF))
        {
            chip.PC += 4;
        }
        else {
            chip.PC += 2;
        }
        break;
    case 0x5000:
        if (chip.V[chip.opcode & 0x0F00] == chip.V[chip.opcode & 0x00F0])
        {
            chip.PC += 4;
        }
        else {
            chip.PC += 2;
        }
        break;
    case 0x6000:
        chip.V[chip.opcode & 0x0F00 >> 8] = chip.opcode & 0x00FF;
        chip.PC += 2;
        break;
    case 0x9000:
        if (chip.V[chip.opcode & 0x0F00] != chip.V[chip.opcode & 0x00F0])
        {
            chip.PC += 2;
        }
        break;
    case 0xA000:
        chip.index = (chip.opcode & 0x0FFF);
        chip.PC += 2;
        break;
    case 0xB000:
        chip.PC = chip.V[0] + (chip.opcode & 0x0FFF);
        break;
    case 0xC000:
        srand(time(NULL));
        chip.V[chip.opcode & 0x0F00] = (rand() % 0x0100) & (chip.opcode & 0x00FF);
        break;
    case 0xE000:
        SDL_Event e;
        switch (chip.opcode & 0x00FF)
        {
        case 0xE09E:
            if(e.key.keysym.sym == KEYMAP[chip.V[chip.opcode & 0x0F00]])
                chip.PC += 2;
            break;
        case 0xE0A1:
            if(e.key.keysym.sym != KEYMAP[chip.V[chip.opcode & 0x0F00]])
                chip.PC += 2;
            break;
        }
        break;
    case 0xF000:
        switch (chip.opcode & 0x00FF)
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
        case 0x0029:
            chip.index = chip.V[chip.opcode & 0x0F00];
            break;
        }
        break;
    case 0x7000:
        chip.V[(chip.opcode & 0x0F00) >> 8] += chip.opcode & 0x00FF;
        //chip.V[chip.opcode & 0x0F00] &= 0xFF;
        chip.PC += 2;
        break;
    case 0x8000:{
        switch (chip.opcode & 0x000F)
        case 0x0: // 8xy0 - LD Vx, Vy
            chip.V[chip.opcode & 0x0F00] = chip.V[chip.opcode & 0x00F0];
            chip.PC += 2;
        break;
        case 0x0001: // 8xy1 - OR Vx, Vy
            chip.V[chip.opcode & 0x0F00] |= chip.V[chip.opcode & 0x00F0];
            chip.PC += 2;
            break;
        case 0x0002: // 8xy2 - AND Vx, Vy
            chip.V[chip.opcode & 0x0F00] &= chip.V[chip.opcode & 0x00F0];
            chip.PC += 2;
            break;
        case 0x0003: // 8xy3 - XOR Vx, Vy
            chip.V[chip.opcode & 0x0F00] ^= chip.V[chip.opcode & 0x00F0];
            chip.PC += 2;
            break;
        case 0x0004: // 8xy4 - ADD Vx, Vy
            chip.V[0xF] = (chip.V[chip.opcode & 0x0F00] + chip.V[chip.opcode & 0x00F0]) > 255 ? 1 : 0;
            chip.V[chip.opcode & 0x0F00] = chip.V[chip.opcode & 0x0F00] + chip.V[chip.opcode & 0x00F0] > 255 ? 1 : 0;
            chip.PC += 2;
            break;
        case 0x0005:
        {
            auto Vy = chip.V[chip.opcode & 0x00F0];
            auto Vx = chip.V[chip.opcode & 0x0F00] > Vy ? 1 : 0;
            Vx = Vx - Vy;
            chip.PC += 2;
            break;
        }
        case 0x0006:
        {
            chip.V[0xF] = chip.V[chip.opcode & 0x0F00] & 1 ? 1 : 0;
            chip.V[chip.opcode & 0x0F00] / 2u;
            chip.PC += 2;
            break;
        }
        case 0x0007:
        {
            auto Vx = chip.V[chip.opcode & 0x0F00];
            auto Vy = chip.V[chip.opcode & 0x00F0] > Vx ? 1 : 0;
            Vx = Vy - Vx;
            chip.PC += 2;
            break;
        }
        case 0x000E:
        {
            chip.V[0xF] = chip.V[chip.opcode & 0x0F00] << 1 ? 1 : 0;
            chip.PC += 2;
            break; // make sure to check this pls
        }
        break;
    }
        
    case 0xD000:
    {
        auto x = chip.V[(chip.opcode & 0x0F00) >> 8];// & 0x00FF;
        auto y = chip.V[(chip.opcode & 0x00F0) >> 4]; //& 0x000F;
        auto n = chip.opcode & 0x000F;
        chip.V[0xF] = 0;
        for (unsigned int i = 0; i < n; i++)
        { // row
            auto sprite = chip.memory[chip.index + i];
            for (unsigned int j = 0; j < 8; j++)
            {                 
                auto pixel = sprite & (0x80 >> j);                     // column
                if (pixel != 0) {
                    if (chip.graphics[(x + i) + ((y + j) * width)] == UINT32_MAX) {
                        chip.V[0xF] = 1;
                        
                    }
                    chip.graphics[(x + j) + ((y + i) * width)] ^= 1;
                }
            }
        }
        chip.draw_flag = true;
        chip.PC +=2;
        break;
    } // display

    }
    if (chip.delay > 0)
    {
        chip.delay--;
    }
    if (chip.sound > 0)
    {
        chip.sound--;
    }
}

void destroySDL() {
    SDL_DestroyRenderer(render);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(screen);
    SDL_Quit();
}

void draw() {
    if(chip.draw_flag){
        uint32_t pixels[width * height];

        for (int i = 0; i < width * height; i++){
            pixels[i] = chip.graphics[i] ? UINT32_MAX : 0x0;
        } 

        SDL_UpdateTexture(texture, nullptr, pixels, width * sizeof(Uint32));
        //SDL_SetRenderDrawColor(render, 255, 255, 0, 255);
        SDL_RenderClear(render);
        SDL_RenderCopy(render, texture, nullptr, nullptr);
        SDL_RenderPresent(render);
        chip.draw_flag = 0;
    }
}

void load_rom(const char *rom)
{

    std::FILE *rom_game;

    rom_game = fopen(rom, "rb");

    if (!rom_game)
    {
        std::cerr << "ROM could not be loaded" << std::endl;
    }

    fseek(rom_game, 0, SEEK_END);
    auto rom_size = ftell(rom_game);
    rewind(rom_game);
    //fseek(rom_game, 0, SEEK_SET);

    if (rom_size > MAX_MEMORY - 0x200)
    {
        std::cerr << "ROM size exceeded max memory" << std::endl;
        fclose(rom_game);
        return;
    }
    unsigned char* buffer = new unsigned char[rom_size];

    fread(buffer, rom_size, 1, rom_game);
    fclose(rom_game);
    //fread(&chip.memory[0x200], 1, MAX_MEMORY, rom_game);
    
    for (int i = 0; i<rom_size; i++){
        chip.memory[i+0x200] = buffer[i];
    }
    delete[] buffer;

}
