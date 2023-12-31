#include "chip.hh"
#include "keymap.hh"
#include <SDL2/SDL.h>
#include <cmath>
#include <random>
#include <fstream>
#include <vector>

Chip8 chip = {};

SDL_Window *screen = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, 
  SDL_WINDOWPOS_CENTERED, 1200, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  | SDL_WINDOW_SHOWN);

SDL_Renderer *render = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);

SDL_Texture *texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,width,height);

uint16_t FONT[FONT_SIZE] = {
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

    chip.delay = 0;
    chip.sound = 0;
    chip.SP = 0;
    chip.PC = 0x200;
    chip.opcode = 0;
    chip.index = 0;
    chip.draw_flag = 0;
    chip.sound_flag = 0;
    memset(chip.memory, 0, sizeof(chip.memory));
    memset(chip.graphics, 0, sizeof(chip.memory));
    memset(chip.V, 0, sizeof(chip.V));
    memset(chip.stack, 0, sizeof(chip.stack));
    memset(chip.keyboard, 0, sizeof(chip.keyboard));
    
    // load sprites into memory
    for (unsigned int i = 0; i < FONT_SIZE; ++i)
    {
        chip.memory[FONT_LOAD + i] = FONT[i];
    }
}

void cycle()
{
    // fetch
    chip.opcode = chip.memory[chip.PC] << 8 | chip.memory[chip.PC + 1];
    chip.PC+=2;
    // decode
    switch (chip.opcode & 0xF000)
    { // check first
    case 0x0000:
        switch (chip.opcode & 0x000F)
        {            
            case 0x0000:
                memset(chip.graphics, 0, sizeof(chip.graphics));
                chip.draw_flag = true;
                
                break;
            case 0x000E:
                chip.PC = chip.stack[chip.SP--];
                
                break;
        }
        break;
    case 0x1000:
        chip.PC = chip.opcode & 0x0FFF;
        break;
    case 0x2000:{
        chip.SP++;
        chip.stack[chip.SP] = chip.PC;
        chip.PC = (chip.opcode & 0x0FFF);
        break;
    }
    case 0x3000:
        if (chip.V[(chip.opcode & 0x0F00) >> 8] == (chip.opcode & 0x00FF))
        {
            chip.PC += 2;
        }
        break;
    case 0x4000:
        if (chip.V[(chip.opcode & 0x0F00) >> 8] != (chip.opcode & 0x00FF))
        {
            chip.PC += 2;
        }
        break;
    case 0x5000:
        if (chip.V[(chip.opcode & 0x0F00) >> 8] == chip.V[(chip.opcode & 0x00F0) >> 4])
        {
            chip.PC += 2;
        }
        break;
    
    case 0x6000:{
        chip.V[(chip.opcode & 0x0F00) >> 8] = chip.opcode & 0x00FF;
        break;
    }
        
    case 0x7000:{
        chip.V[(chip.opcode & 0x0F00) >> 8] += chip.opcode & 0x00FF;
        break;
    }
    case 0x8000:{
        auto x = (chip.opcode & 0x0F00) >> 8;
        auto y = (chip.opcode & 0x00F0) >> 4;
        switch (chip.opcode & 0x000F){
            case 0x0000: // 8xy0 - LD Vx, Vy {}
                chip.V[x] = chip.V[y];
                
            break;

            case 0x0001: // 8xy1 - OR Vx, Vy
                chip.V[x] |= chip.V[y];
                
            break;

            case 0x0002: // 8xy2 - AND Vx, Vy
                chip.V[x] &= chip.V[y];
                
            break;

            case 0x0003: // 8xy3 - XOR Vx, Vy
                chip.V[x] ^= chip.V[y];
            break;

            case 0x0004:{ // 8xy4 - ADD Vx, Vy
                auto carry = chip.V[x] + chip.V[y] > 255 ? 1 : 0;
                
                chip.V[x] = (chip.V[x] + chip.V[y]) & 0xFF;
                chip.V[0xF] = carry;
                break;
            }
            case 0x0005:
            {
                auto carry = chip.V[x] > chip.V[y] ? 1 : 0;
                
                chip.V[x] -= chip.V[y];
                chip.V[0xF] = carry;
                break;
            }

            case 0x0006:
            {
                auto carry = chip.V[x] & 0x0001 ? 1 : 0;
                chip.V[x] >>= 1;
                chip.V[0xF] = carry;
                
                break;
            }

            case 0x0007:
            {
                auto carry = chip.V[y] >= chip.V[x] ? 1 : 0;
                chip.V[x] = chip.V[y] - chip.V[x];
                chip.V[0xF] = carry;
                break;
            }
            case 0x000E:
            {
                auto carry = chip.V[x] >> 7; //most sig bit to right
                chip.V[x] <<= 1;
                chip.V[0xF] = carry;
                break;
            }
        }

      break;
    }
    case 0x9000:
        if (chip.V[(chip.opcode & 0x0F00) >> 8] != chip.V[(chip.opcode & 0x00F0) >> 4])
        {
            chip.PC += 2;
        }
        break;
    case 0xA000:{
        chip.index = (chip.opcode & 0x0FFF);
        break;
    }
    case 0xB000:
        chip.PC = chip.V[0] + (chip.opcode & 0x0FFF);
        break;
    case 0xC000:
        chip.V[(chip.opcode & 0x0F00) >> 8] = rand() & (chip.opcode & 0x00FF);
        break;
    case 0xD000:
    {
        auto x = chip.V[(chip.opcode & 0x0F00) >> 8];
        auto y = chip.V[(chip.opcode & 0x00F0) >> 4];
        auto n = chip.opcode & 0x000F;
        chip.V[0xF] = 0;
        for (unsigned int i = 0; i < n; i++)
        { // row
            auto sprite = chip.memory[chip.index + i];
            for (unsigned int j = 0; j < 8; j++)
            {                 
                auto pixel = sprite & (0x80 >> j);
                if (pixel != 0) {
                    if (chip.graphics[(x + i) + ((y + j) * width)] == UINT32_MAX) {
                        chip.V[0xF] = 1;

                    }
                    chip.graphics[(x + j) + ((y + i) * width)] ^= 1;
                }
            }
        }
        chip.draw_flag = true;
        break;
    } // display
    case 0xE000:
        switch (chip.opcode & 0x00FF)
        {
        case 0x009E:
            if(chip.keyboard[chip.V[(chip.opcode & 0x0F00) >> 8]])
                chip.PC += 2;
            break;
        case 0x00A1:
            if(!(chip.keyboard[chip.V[(chip.opcode & 0x0F00) >> 8]]))
                chip.PC += 2;
            break;
        }
        break;
    case 0xF000:
        switch (chip.opcode & 0x00FF)
        {
            case 0x0007: //LD Vx, DT
                chip.V[(chip.opcode & 0x0F00) >> 8] = chip.delay;
                break;
            case 0x000A: {
                bool is_key_pressed = false;

                for (int i = 0; i < KEY_COUNT; i++) {
                    if (chip.keyboard[i] != 0) {
                        chip.keyboard[i] = 0;
                        chip.V[(chip.opcode & 0x0F00) >> 8] = i;
                        is_key_pressed = true;
                        break;
                    }
                }
                if (is_key_pressed == false) {
                    chip.PC-=2;
                }
            break;    
            }
            
            case 0x0015:
                chip.delay = chip.V[(chip.opcode & 0x0F00) >> 8];
                break;
            case 0x0018:
                chip.sound = chip.V[(chip.opcode & 0x0F00) >> 8];
                chip.sound_flag = true;
                break;
            case 0x001E:
                chip.index += chip.V[(chip.opcode & 0x0F00) >> 8];
                
                break;
            case 0x0029:
                chip.index = FONT_LOAD + (5 * chip.V[(chip.opcode & 0x0F00) >> 8]);
                
                break;
            case 0x0055:
                for (int i = 0; i <= (chip.opcode & 0x0F00) >> 8; i++) {
                    chip.memory[chip.index + i] = chip.V[i];
                }
                
                break;
            case 0x0065:
                for (int i = 0; i <= (chip.opcode & 0x0F00) >> 8; i++) {
                    chip.V[i] = chip.memory[chip.index + i];
                }
                
                break;
            case 0x0033: {
                auto regx = chip.V[(chip.opcode & 0x0F00) >> 8];
                chip.memory[chip.index] = regx / 100;
                chip.memory[chip.index + 1] = (regx / 10) % 10;
                chip.memory[chip.index + 2] = (regx % 100) % 10;
                
                break;
            }
        }
        break;
     
        
    
    break;}
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
            pixels[i] = chip.graphics[i] ? 0x9B0039 : 0x27285C;
        } 

        SDL_UpdateTexture(texture, nullptr, pixels, width * sizeof(Uint32));
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

void getInput(SDL_Event e, bool& running)
{
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            break;
        }
        else if(e.type == SDL_KEYDOWN)
        {
            for(int i = 0; i < KEY_COUNT; i++)
            {
                if(e.key.keysym.sym == KEYMAP[i])
                    chip.keyboard[i] = 1;
            }
            if(e.key.keysym.sym == SDLK_ESCAPE){
                running = false;
                break;
            }
        }
        else if(e.type == SDL_KEYUP)
        {
            for(int i = 0; i < KEY_COUNT; i++)
            {
                if(e.key.keysym.sym == KEYMAP[i])
                    chip.keyboard[i] = 0;
            }
        }
    }
}

void beepboop(){
    if ((chip.sound > 0) && (chip.sound_flag == true)) {
    SDL_Init(SDL_INIT_AUDIO);
    
    SDL_AudioSpec sound;
    sound.freq = 8000;
    sound.format = AUDIO_S16;
    sound.channels = 1;
    sound.samples = 1028;
    sound.callback = NULL;
    sound.userdata = NULL;
    int x = 0;
    SDL_AudioDeviceID audio = SDL_OpenAudioDevice(NULL, 0, &sound, nullptr, 0);
    for(int i = 0; i < sound.freq; i++) {
        x+=.010f;
        double sampleValue = 900 * sin(2.0 * M_PI);
        auto sample = static_cast<Sint16>(sampleValue);
        SDL_QueueAudio(audio, &sample, sizeof(double) * 10);
    }
    
    SDL_PauseAudioDevice(audio, 0);
    SDL_Delay(100);
    SDL_PauseAudioDevice(audio, 1);
    SDL_CloseAudio();
    chip.sound_flag = false;
    }

}
