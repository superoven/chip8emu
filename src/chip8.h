#ifndef __chip8_h_
#define __chip8_h_

#include <stdio.h>

#define WIDTH 64
#define HEIGHT 32

#define RAMLENGTH 4096

#define DEBUGMODE 0

#define MODIFIER 10

#define TIMERLENGTH 16666

extern int display_width;
extern int display_height;

extern int HARNESS;

//The rom itself
char* romdata;

//The processor structure
typedef struct
{
  unsigned char V[16];
  unsigned short pc;
  unsigned short I;
  unsigned char soundtimer;
  unsigned char delaytimer;
  unsigned char drawflag;
  unsigned char sp;
  unsigned short stack[16];
} processor_t;

//Subset of instruction type to parse nibbles
typedef struct {
  unsigned int nib2 : 4;
  unsigned int nib1 : 4;
  unsigned int nib0 : 4;
  unsigned int opcode : 4;
} ntype_inst_t;

//Subset of instruction type to parse jumps
typedef struct {
  unsigned int address : 12;
  unsigned int opcode : 4;
} jtype_inst_t;

//Subset of instruction type to parse intermediates
typedef struct {
  unsigned int imm : 8;
  unsigned int reg : 4;
  unsigned int opcode : 4;
} itype_inst_t;

//Instruction Type to allow for multiple interpretations
typedef union {
  ntype_inst_t ntype;
  jtype_inst_t jtype;
  itype_inst_t itype;
  unsigned short bits;
} inst_t;

//Current Instruction
inst_t inst;

//RAM 4k total
unsigned char mem[RAMLENGTH];

//Graphics memory
extern unsigned gfx[WIDTH * HEIGHT];

//Index Register
unsigned short I;

//Keypad for memory mapped I/O
unsigned char key[16];

//Fontset for the processor
unsigned char fontset[80];

//Load a given "romname" into the pointer starting at "result"
int loadrom(const char* romname, char** result);

//Throw an error and exit
void error(const char* message);

//Initialize all data in the processor and screen
void initialize(processor_t *p, int size);

//Fetch the instruction at the pc
inst_t fetch();

//Run a cpu cycle
void cpucycle(processor_t* p);

//Disassemble instruction
void disassemble(inst_t a, FILE* fp);

//Print registers for debugging
void printreg(processor_t *p);

void run(processor_t *p);

//Glut draw function
void drawScreen();
void keyboardDown(unsigned char val, int x, int y);
void keyboardUp(unsigned char val, int x, int y);

void drawscreentext();
void reshapeWindow();

void decTimers(int value);

#endif
