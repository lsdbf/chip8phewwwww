#include <SDL2/SDL.h>
#include <iostream>
#include "chip.hh"

int main() {

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

  SDL_SetRenderDrawColor(render,1,0,0,1);
  SDL_RenderPresent(render);
  
  while (true) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}
  }
  SDL_DestroyWindow(screen);
  SDL_Quit();
  return EXIT_SUCCESS;
}
