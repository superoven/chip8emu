#include <stdio.h>
#include <time.h>
#include "SDL/SDL.h"
#include "chip8.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: ./chipemu [rom]\n");
    return 0;
  }

  srand(time(NULL));
  
  int filesize = loadrom(argv[1], &romdata);

  initialize(filesize);

  //SDL_Init(SDL_INIT_EVERYTHING);
  //SDL_Quit();
  return 0;
}
