#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "chip8.h"

char* romdata;

unsigned char fontset[80] =
  { 
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
  };

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

void error(const char* message) {
  fprintf(stderr, "Error: %s\n", message);
  exit(1);
}

void initialize(int size) {
  pc = 0x200;
  inst.bits = 0;
  I = 0;
  sp = 0;
  soundtimer = 0;
  delaytimer = 0;
  drawflag = 0;
  
  //Clear display
  //Clear stack
  //Clear registers
  //Clear memory
  
  //Set up fontset
  for(int i = 0; i < 80; ++i) mem[i] = fontset[i];

  //Put rom data into memory
  for(int i = 0; i < size; i++) mem[i + 0x200] = romdata[i];


  srand(time(NULL));
  
  free(romdata);
}

inst_t fetch() {
  inst_t ret;
  ret.bits = mem[pc] << 8 | mem[pc + 1];
  pc += 2;
  return ret;
}
