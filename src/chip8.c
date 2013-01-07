#include <stdio.h>
#include "SDL/SDL.h"
#include "chip8.h"

SDL_Surface *screen;
SDL_Event event;

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage: ./chipemu [rom]\n");
    return 0;
  }

  int filesize = loadrom(argv[1], &romdata);

  initialize(filesize);

  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(WIDTH*MULT, HEIGHT*MULT, 16, SDL_HWSURFACE);
  SDL_WM_SetCaption("Simple Window", "Simple Window");
  char c=getchar();
  SDL_Quit();
  return 0;
}
