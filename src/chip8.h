#ifndef __processor_h_
#define __processor_h_

//Current Instruction
unsigned short inst;

//RAM 4k total
unsigned char mem[4096];

//Registers
unsigned char V[16];

//Index Register
unsigned short I;

//Program Counter
unsigned short pc;

//Graphics array (Resolution: 64 x 32)
unsigned gfx[64*32];

//Timers for Syncing
unsigned char soundtimer;
unsigned char delaytimer;

//Stack for function backtracking
unsigned short stack[16];
unsigned short sp;

//Keypad for memory mapped I/O
unsigned char key[16];

#endif
