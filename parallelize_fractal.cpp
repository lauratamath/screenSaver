
#include <iostream>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <complex>
// #include <omp.h>
#include <string>

const int WIDTH = 640, HEIGHT = 480, MAX_ITERATIONS = 10;
// const double minBound = -4.0, maxBound = 4.0;
double funcC = 1.0;
double a = 0.1;


std::complex<double> func(std::complex<double> x) {
  // std::complex<double> realUnit(1,0);
  return x * x * x - funcC; // x^3 - 1
}

std::complex<double> deriv(std::complex<double> x) {
  // std::complex<double> realUnit(3,0);
  return 3.0 * x * x; // 3 x^2
}

std::complex<double> rootOne(1,0);
std::complex<double> rootTwo(-0.5, sqrt(3)/2);
std::complex<double> rootThree(-0.5, -sqrt(3)/2);

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_Window *window = SDL_CreateWindow("Newtons Fractal", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);;
  
  // SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS, &window, &renderer);

  if (NULL == window) {
    std::cout << "Could not create window: "<< SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Event windowEvent;

  bool running = true;

  // SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
  
  // uint32_t* pixels = new uint32_t[WIDTH * HEIGHT];


  // Lifecicle
  while ( running ) {

    // TODO: calculate and show fps
    double start_time = SDL_GetTicks();
    if (SDL_PollEvent(&windowEvent)) {
      switch (windowEvent.type) {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_MOUSEBUTTONDOWN:
          if (windowEvent.button.button == SDL_BUTTON_RIGHT) {
            // Zoom in
          }
          else if (windowEvent.button.button == SDL_BUTTON_LEFT) {
            // Zoom out
          }
          break;
        case SDL_KEYDOWN:
          if (windowEvent.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
          }
          break;
      }
    }

    // avoiding the overhead of allocating and deallocating memory repeatedly with memory pooling

    // allocate memory for pixels
    uint32_t* pixels = new uint32_t[WIDTH * HEIGHT];

    // iterate each pixel
    #pragma omp parallel for
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        int x = i % WIDTH;
        int y = i / WIDTH;

        // scaled_value = (value - min) / (max - min) * (new_max - new_min) + new_min
        double zx = (1.0 * x - WIDTH / 2.0) / (WIDTH / 4.0);
        double zy = (1.0 * y - HEIGHT / 2.0) / (HEIGHT / 4.0);

        std::complex<double> z(zx, zy);

        double tolerance = 0.01;

        int iter = 0;
        int r = 0, g = 0, b = 0;
        uint32_t color = 0;

        while (iter < MAX_ITERATIONS) {
            z -= a * (func(z) / deriv(z));

            std::complex<double> rdiff = z - rootOne;
            std::complex<double> gdiff = z - rootTwo;
            std::complex<double> bdiff = z - rootThree;

            if (std::norm(rdiff.real()) < tolerance && std::norm(rdiff.imag()) < tolerance) {
                r = 255 - iter % 255;
                break;
            }
            else if (std::norm(gdiff.real()) < tolerance && std::norm(gdiff.imag()) < tolerance) {
                g = 255 - iter % 255;
                break;
            }
            else if (std::norm(bdiff.real()) < tolerance && std::norm(bdiff.imag()) < tolerance) {
                b = 255 - iter % 255;
                break;
            }
            iter++;
        }

        // r, g, and b into a single 32-bit color value
        color |= (r << 16) | (g << 8) | b;

        // allocate color in the pixels array
        pixels[i] = color;
    }

    // Create a texture from the pixels array
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    void* texture_pixels;
    int pitch;

    SDL_LockTexture(texture, NULL, &texture_pixels, &pitch);
    memcpy(texture_pixels, pixels, sizeof(uint32_t) * WIDTH * HEIGHT);
    SDL_UnlockTexture(texture);

    // Render the texture
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Free memory
    delete[] pixels;


    // funcC += 0.01;
    a += 0.01;
    
    // Debug
    // std::cout << "A: " << a << std::endl;

    double end_time = SDL_GetTicks();
    double elapsed_time = end_time - start_time;
    double fps = 1000.0 / elapsed_time;
    std::cout << "FPS: " << fps << std::endl;
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}


