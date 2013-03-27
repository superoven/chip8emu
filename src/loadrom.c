#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include <string.h>

#include "chip8.h"

int HARNESS;
char* romdata;
unsigned gfx[WIDTH * HEIGHT];
int display_width;
int display_height;

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
  for(int i = 0; i < 14; i += 2) {
    printf("V[%-2d] = 0x%02X    V[%-2d] = 0x%02X\n", i, p->V[i], i+1, p->V[i+1]);
  }
  printf("V[14] = 0x%02X\n", p->V[14]);
  printf("\nV[F ] = 0x%02X\n", p->V[15]);
  printf("I %-3s = 0x%04X\n", "", p->I);
}

void printGFXBin() {
  for(int i = 0; i < WIDTH * HEIGHT; i++) {
    printf(((gfx[i]) ? "1" : "0"));
  }
  printf("\n");
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
  p->I = 0;
  p->soundtimer = 0;
  p->delaytimer = 0;
  p->drawflag = 0;
  p->sp = 0;  

  //Clear stack
  //Clear registers
  for(int i = 0; i < 16; i++) {
    p->stack[i] = 0;
    p->V[i] = 0;
  }

  //Clear memory
  for(int i = 0; i < RAMLENGTH; i++) mem[i] = 0;

  //Clear graphics buffer
  for(int i = 0; i < (64*32); i++) gfx[i] = 0;
  
  //Set up fontset
  for(int i = 0; i < 80; ++i) mem[i] = fontset[i];

  //Put rom data into memory
  for(int i = 0; i < size; i++) mem[i + 0x200] = romdata[i];

  //Seed the random number generator
  srand(time(NULL));
  
  //Remove rom memory now that it has been copied
  free(romdata);
}

inst_t fetch(processor_t *p) {
  inst_t ret;
  ret.bits = ((mem[p->pc]&0xFF) << 8) | (mem[p->pc + 1]&0xFF);
  return ret;
}

void keyboardDown(unsigned char val, int x, int y)
{
  if(val == 27) exit(0);

  if(val == '1')        key[0x0] = 1;
  else if(val == '2')	key[0x1] = 1;
  else if(val == '3')	key[0x2] = 1;
  else if(val == '4')	key[0x3] = 1;

  else if(val == 'q')	key[0x4] = 1;
  else if(val == 'w')	key[0x5] = 1;
  else if(val == 'e')	key[0x6] = 1;
  else if(val == 'r')	key[0x7] = 1;

  else if(val == 'a')	key[0x8] = 1;
  else if(val == 's')	key[0x9] = 1;
  else if(val == 'd')	key[0xA] = 1;
  else if(val == 'f')	key[0xB] = 1;

  else if(val == 'z')	printGFXBin();
  else if(val == 'x')	key[0xD] = 1;
  else if(val == 'c')	key[0xE] = 1;
  else if(val == 'v')	key[0xF] = 1;
}

void keyboardUp(unsigned char val, int x, int y)
{
  if(val == '1')	key[0x0] = 0;
  else if(val == '2')	key[0x1] = 0;
  else if(val == '3')	key[0x2] = 0;
  else if(val == '4')	key[0x3] = 0;

  else if(val == 'q')	key[0x4] = 0;
  else if(val == 'w')	key[0x5] = 0;
  else if(val == 'e')	key[0x6] = 0;
  else if(val == 'r')	key[0x7] = 0;

  else if(val == 'a')	key[0x8] = 0;
  else if(val == 's')	key[0x9] = 0;
  else if(val == 'd')	key[0xA] = 0;
  else if(val == 'f')	key[0xB] = 0;

  else if(val == 'z')	key[0xC] = 0;
  else if(val == 'x')	key[0xD] = 0;
  else if(val == 'c')	key[0xE] = 0;
  else if(val == 'v')	key[0xF] = 0;
}

void run(processor_t* p) {
  cpucycle(p);
  if (p->drawflag) {
    p->drawflag = 0;
    drawscreentext();
  }
}

void drawscreentext() {
  if (HARNESS) return;
  char buffer[(32*64) + 32*2];
  for(int j = 0; j < HEIGHT; j++) {
    for(int i = 0; i < WIDTH+1; i++) {
      if (i == WIDTH) buffer[i+(j*(WIDTH+1))] = '\n';
      else buffer[i+(j*(WIDTH+1))] = (gfx[i + (j*WIDTH)]) ? '*' : ' ';
      //printf((gfx[i + (j*WIDTH)]) ? " " : "*");
      //printf((i == WIDTH - 1) ? "\n" : "");
    }
  }
  //int loc = WIDTH + 1 + (HEIGHT * (WIDTH+1));
  printf("%s",buffer);
  printf("\033[2J\033[1;1H");
}

void reshapeWindow(GLsizei width, GLsizei height)
{
  glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, height, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, width, height);

  display_width = width;
  display_height = height;

}

void drawScreen() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (gfx[x+(y*WIDTH)] == 0) {
	glColor3f(0.0f, 0.0f, 0.0f);
      } else {
	glColor3f(1.0f, 1.0f, 1.0f);
      }

      glBegin(GL_QUADS);	

      glVertex3f((x*MODIFIER), (y*MODIFIER), 0.0f);
      glVertex3f((x*MODIFIER), (y*MODIFIER) + MODIFIER, 0.0f);
      glVertex3f((x*MODIFIER) + MODIFIER, (y*MODIFIER) + MODIFIER, 0.0f);
      glVertex3f((x*MODIFIER) + MODIFIER, (y*MODIFIER), 0.0f);

      glEnd();
    }
  }
  glutSwapBuffers();
}
