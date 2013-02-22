#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>

void usagemessage() {
  printf("Usage: ./chipdis inputfile [-o outputfile]\n");
  exit(0);
}

int main(int argc, char * argv[]) {
  if (argc < 2) usagemessage();
  return 0;
}
 
