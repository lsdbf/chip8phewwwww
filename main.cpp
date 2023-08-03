#include <SDL2/SDL.h>
#include <iostream>
#include "chip.hh"

/*SDL_Window *screen = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, 
  SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  | SDL_WINDOW_SHOWN);

SDL_Renderer *render = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);

SDL_Texture *texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,width,height);*/
  

int main(int argc, char** argv) {
  
  initialize();

  const char *rom_path = "ROMS/Trip8 Demo (2008) [Revival Studios].ch8";
  /**/

  load_rom(rom_path);
  SDL_Event e;
  bool running = 1;
  while (running) {
    auto start_time = SDL_GetTicks(); //make this into a switch statement
    cycle();

    draw();

    getInput(e, running);

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
  destroySDL();
  return 0;
}
