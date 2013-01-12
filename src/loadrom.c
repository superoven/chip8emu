#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>

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

void printreg(processor_t *p) {
  for(int i = 0; i < 16; i++) {
    printf("V[%-2d] = 0x%04X\n", i, p->V[i]);
  }
  printf("I = 0x%04X\n", I);
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

void error(const char* message) {
  fprintf(stderr, "Error: %s\n", message);
  exit(1);
}

void initialize(processor_t *p, int size) {
  p->pc = 0x200;
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

inst_t fetch(processor_t *p) {
  inst_t ret;
  ret.bits = mem[p->pc] << 8 | mem[p->pc + 1];
  p->pc += 2;
  return ret;
}

void drawPixel(int x, int y)
{
  glBegin(GL_QUADS);
  glVertex3f((x * MULT) + 0.0f, (y * MULT) + 0.0f, 0.0f);
  glVertex3f((x * MULT) + 0.0f, (y * MULT) + MULT, 0.0f);
  glVertex3f((x * MULT) + MULT, (y * MULT) + MULT, 0.0f);
  glVertex3f((x * MULT) + MULT, (y * MULT) + 0.0f, 0.0f);
  glEnd();
}

void updateQuads()
{
  for(int y = 0; y < 32; ++y)		
    for(int x = 0; x < 64; ++x)
      {
	if(gfx[(y*64) + x] == 0) 
	  glColor3f(0.0f,0.0f,0.0f);			
	else 
	  glColor3f(1.0f,1.0f,1.0f);

	drawPixel(x, y);
      }
}

void keyboardDown(unsigned char val, int x, int y)
{
  if(val == 27) exit(0);

  if(val == '1')        key[0x1] = 1;
  else if(val == '2')	key[0x2] = 1;
  else if(val == '3')	key[0x3] = 1;
  else if(val == '4')	key[0xC] = 1;

  else if(val == 'q')	key[0x4] = 1;
  else if(val == 'w')	key[0x5] = 1;
  else if(val == 'e')	key[0x6] = 1;
  else if(val == 'r')	key[0xD] = 1;

  else if(val == 'a')	key[0x7] = 1;
  else if(val == 's')	key[0x8] = 1;
  else if(val == 'd')	key[0x9] = 1;
  else if(val == 'f')	key[0xE] = 1;

  else if(val == 'z')	key[0xA] = 1;
  else if(val == 'x')	key[0x0] = 1;
  else if(val == 'c')	key[0xB] = 1;
  else if(val == 'v')	key[0xF] = 1;
}

void keyboardUp(unsigned char val, int x, int y)
{
  if(val == '1')	key[0x1] = 0;
  else if(val == '2')	key[0x2] = 0;
  else if(val == '3')	key[0x3] = 0;
  else if(val == '4')	key[0xC] = 0;

  else if(val == 'q')	key[0x4] = 0;
  else if(val == 'w')	key[0x5] = 0;
  else if(val == 'e')	key[0x6] = 0;
  else if(val == 'r')	key[0xD] = 0;

  else if(val == 'a')	key[0x7] = 0;
  else if(val == 's')	key[0x8] = 0;
  else if(val == 'd')	key[0x9] = 0;
  else if(val == 'f')	key[0xE] = 0;

  else if(val == 'z')	key[0xA] = 0;
  else if(val == 'x')	key[0x0] = 0;
  else if(val == 'c')	key[0xB] = 0;
  else if(val == 'v')	key[0xF] = 0;
}

void display()
{
  cpucycle();

  /*if(drawflag)
    {
      // Clear framebuffer
      glClear(GL_COLOR_BUFFER_BIT);
        
      updateQuads();		

      // Swap buffers!
      glutSwapBuffers();    

      // Processed frame
      drawflag = 0;
      }*/
}
