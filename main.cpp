#include <SDL2/SDL.h>
#include <iostream>
#include "chip.hh"


int main(/*int argc, char** argv*/) {
  

  initialize();
  /*if (argc != 2) {
    std::cerr << "Failed to load ROM" << std::endl;
  }*/
  SDL_Init(SDL_INIT_EVERYTHING);

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
    return 3;
}

  const char *rom_path = "IBM Logo.ch8";

  SDL_Window *screen = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, 
  SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  | SDL_WINDOW_SHOWN);
  if (screen == NULL) {
    std::cout << "ERROR: COULD NOT CREATE SCREEN" << std::endl;
    return 3;
  }
  SDL_Renderer *render = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
  if (render == NULL) {
    std::cout << "ERROR: COULD NOT CREATE RENDERER" << std::endl;
    return 3;
  }
  SDL_Texture *texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,width,height);
  if (texture == NULL){
    std::cout << "ERROR: COULD NOT CREATE TEXTURE" << std::endl;
    return 3;
  }
  Chip8 chip8;
  int pitch = sizeof(chip8.graphics[0]);
  
  load_rom(rom_path);
  SDL_Event e;
  bool running = 1;
  while (running) {
    auto start_time = SDL_GetTicks(); //make this into a switch statement
    cycle();
    SDL_UpdateTexture(texture, nullptr, chip8.graphics,pitch);
    SDL_RenderClear(render);
    SDL_RenderCopy(render, texture, nullptr, nullptr);
    SDL_RenderPresent(render);

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            break;
        }
        else if (e.type == SDL_KEYDOWN) {
          if (e.key.keysym.sym == SDLK_ESCAPE) {
            running = 0;
            break;
          }
        }
    }
    auto end = SDL_GetTicks();
    auto elasped = end - start_time;
    if (elasped < (1000/60)) {
      SDL_Delay((1000/60) - elasped);
    }
    
  }
  //clean
  SDL_DestroyRenderer(render);
  SDL_DestroyTexture(texture);
  SDL_DestroyWindow(screen);
  SDL_Quit();
  return EXIT_SUCCESS;
}
