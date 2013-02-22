#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_OUT "a.out"

void usage() {
  printf("Usage: ./chipdis inputfile [outputfile]\n");
  exit(0);
}

int loadrom(const char* romname, char** result) {
  int size;
  FILE* fp;
  fp = fopen(romname, "rb");
  if (fp == NULL) error("File cannot be opened");
  
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  *result = (char *)malloc(size);

  if (size != fread(*result, sizeof(char), size, fp)) 
  { 
    free(*result);
    error("File buffered incorrectly");
  }

  fclose(fp);
  return size;
}

//Load and disassemble "in" into "out"
void disto(const char* in, const char* out) {
  char* romin;
  int filesize = loadrom(in, &romin);

  FILE* outf;
  outf = fopen(out, "wb");
  if (outf == NULL) error("File cannot be opened");
  
  inst_t inst;
  for(int i = 0; i < filesize; i += 2) {
    inst.bits = romin[i] << 8 | romin[i+1];
    disassemble(inst, outf);
  }
  
  free(romin);
  //free(outf);
}

int main(int argc, char * argv[]) {
  if (argc < 2) usage();
  else if (argc == 2) disto(argv[1], DEFAULT_OUT);
  else if (argc == 3) disto(argv[1], argv[2]);
  else usage();
  return 0;
}
 
