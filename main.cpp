#include <SDL.h>
#include <iostream>
#include "chip.hh"

int main(int argc, char* args[]) {
  
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

  while (true) { //make this into a switch statement
		SDL_Event e;
    SDL_SetRenderDrawColor(render,0,0,0,0);
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
      if (e.key.keysym.sym == SDLK_d){
        SDL_SetRenderDrawColor(render, 27,52,127,SDL_ALPHA_OPAQUE); //background
        SDL_RenderClear(render);

        //SDL_SetRenderDrawColor(render,180,189,25,SDL_ALPHA_OPAQUE); //line color
        //SDL_RenderDrawLine(render, 0, 60, 60, -60);

        SDL_RenderPresent(render);
      }
		}
    
  }
  SDL_DestroyRenderer(render);
  SDL_DestroyWindow(screen);
  SDL_Quit();
  return EXIT_SUCCESS;
}
