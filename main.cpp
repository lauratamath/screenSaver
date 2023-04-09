#include <iostream>
#include <stdlib.h>
#include <SDL2\SDL.h>
#include <complex>

const int WIDTH = 800, HEIGHT = 600;

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_Window *window;
  SDL_Renderer *renderer;
  
  SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_BORDERLESS, &window, &renderer);

  if (NULL == window) {
    std::cout << "Could not create window: "<< SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Event windowEvent;

  // Lifecicle
  // while ( true ) {
  //   if (SDL_PollEvent(&windowEvent)) {
  //     if (SDL_QUIT == windowEvent.type) {
  //       break;
  //     }
  //   }
  // }

  for (int x = 0; x < HEIGHT; ++x ) {
    for (int y =0 ; y < WIDTH; ++y) {
      std::complex<double> c(1.0 * x / HEIGHT * 4.0 - 2.0, 1.0 * y / HEIGHT * 4.0 - 2.0);
      std::complex<double> z(0.0, 0.0);
      int count = 0;

      while (count < 255) {
        z = z * z + c;
        if (abs(z) > 2.0) {
          break;
        }
        ++count;
      }

      SDL_SetRenderDrawColor(renderer, count, count, count, 0xff);
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }

  SDL_RenderPresent(renderer);
  SDL_Delay(2000);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}

