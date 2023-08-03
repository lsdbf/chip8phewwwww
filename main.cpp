#include <SDL2/SDL.h>
#include <iostream>
#include "chip.hh"

int main(int argc, char** argv) {
  
  initialize();
  
  const char *rom_path = "ROMS/quirks.ch8";
  /**/
  load_rom(rom_path);
  SDL_Event e;
  bool running = 1;
  while (running) {
    //auto start_time = SDL_GetTicks();
    cycle();
    draw();
    beepboop();
    getInput(e, running);

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = 0;
            break;
        }
        else if (e.type == SDL_KEYDOWN) {
          if (e.key.keysym.sym == SDLK_ESCAPE) {
            running = 0;
            break;
          }
        }
    }
    /*auto end = SDL_GetTicks();
    auto elasped = end - start_time;
    if (elasped < (1000/400)) {
      SDL_Delay((1000/400) - elasped);
    }*/


    
  }
  //clean
  destroySDL();
  return 0;
}
