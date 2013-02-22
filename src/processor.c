#include "chip8.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int HARNESS;
unsigned gfx[64*32];

void postvisit(processor_t* p, int jump) {
  if (!jump) p->pc += 2;
  if (p->delaytimer) --p->delaytimer;
  if(p->soundtimer){
    if(p->soundtimer == 1 && !HARNESS) printf("BEEP!\n");
    --p->soundtimer;
  }
}

void debugout (processor_t* p, inst_t inst) {
  printf("\nPC: 0x%04X  INST: 0x%04X\n", p->pc, inst.bits);
  disassemble(inst, stdout);
  printreg(p);
  printf("\nDelayTimer: %d\nSoundTimer: %d\n", p->delaytimer, p->soundtimer);
  printf("Drawflag: %s\n", (p->drawflag ? "True" : "False"));
  printf("sp: 0x%04X\n", p->sp);
}

void cpucycle(processor_t* p) {

  inst = fetch(p);

  if (DEBUGMODE) debugout(p, inst);

  p->drawflag = 0;

  switch(inst.ntype.opcode) {

    case 0x0:
    switch(inst.ntype.nib2) {

    case 0x0: //00E0: Clear Screen
      for(int i = 0; i < 2048; i++) gfx[i] = 0;
      p->drawflag = 1;
      postvisit(p,0);
      return;

    case 0xE: //00EE: Return from subroutine
      --p->sp;
      p->pc = p->stack[p->sp];
      postvisit(p,0);
      return;
    }

  case 0x1: //1NNN Jump to address NNN
    p->pc = inst.jtype.address;
    postvisit(p,1);
    return;

  case 0x2: //2NNN Call subroutine at NNN
    p->stack[p->sp] = p->pc;
    ++p->sp;
    p->pc = inst.jtype.address;
    postvisit(p,1);
    return;

  case 0x3: //3XNN Skips the next instruction if VX equals NN.
    if (p->V[inst.ntype.nib0] == inst.itype.imm) p->pc += 2;
    postvisit(p,0);
    return;

  case 0x4: //4XNN Skips the next instruction if VX doesn't equal NN.
    if (p->V[inst.ntype.nib0] != inst.itype.imm) p->pc += 2;
    postvisit(p,0);
    return;

  case 0x5: //5XY0 Skips the next instruction if VX equals VY.
    if (p->V[inst.ntype.nib0] == p->V[inst.ntype.nib1]) p->pc += 2;
    postvisit(p,0);
    return;

  case 0x6: //6XNN Sets VX to NN.
    p->V[inst.ntype.nib0] = inst.itype.imm;
    postvisit(p,0);
    return;

  case 0x7: //7XNN Adds NN to VX.
    p->V[inst.ntype.nib0] += inst.itype.imm;
    postvisit(p,0);
    return;

  case 0x8: //Arithmetic things
    switch(inst.ntype.nib2) {
      
    case 0x0: //8XY0 Sets VX to the value of VY.
      p->V[inst.ntype.nib0] = p->V[inst.ntype.nib1];
      postvisit(p,0);
      return;

    case 0x1: //8XY1 Sets VX to VX or VY.
      p->V[inst.ntype.nib0] = p->V[inst.ntype.nib0] | p->V[inst.ntype.nib1];
      postvisit(p,0);
      return;
      
    case 0x2: //8XY2 Sets VX to VX and VY.
      p->V[inst.ntype.nib0] = p->V[inst.ntype.nib0] & p->V[inst.ntype.nib1];
      postvisit(p,0);
      return;

    case 0x3: //8XY3 Sets VX to VX xor VY.
      p->V[inst.ntype.nib0] = p->V[inst.ntype.nib0] ^ p->V[inst.ntype.nib1];
      postvisit(p,0);
      return;
    
    case 0x4: //8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
      p->V[0xF] = (p->V[inst.ntype.nib1] > (0xFF - p->V[inst.ntype.nib0])) ? 1 : 0;
      p->V[inst.ntype.nib0] += p->V[inst.ntype.nib1];
      postvisit(p,0);
      return;

    case 0x5: //8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      p->V[0xF] = (p->V[inst.ntype.nib0] > p->V[inst.ntype.nib1]) ? 1 : 0;
      p->V[inst.ntype.nib0] = p->V[inst.ntype.nib0] - p->V[inst.ntype.nib1];
      postvisit(p,0);
      return;
      
    case 0x6: //8XY6 Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
      p->V[0xF] = p->V[inst.ntype.nib0] & 1;
      p->V[inst.ntype.nib0] >>= 1;
      postvisit(p,0);
      return;

    case 0x7: //8XY7 Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      p->V[0xF] = (p->V[inst.ntype.nib1] > p->V[inst.ntype.nib0]) ? 1 : 0;
      p->V[inst.ntype.nib0] = p->V[inst.ntype.nib1] - p->V[inst.ntype.nib0];
      postvisit(p,0);
      return;

    case 0xE: //8XYE Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
      p->V[0xF] = p->V[inst.ntype.nib0] >> 7;
      p->V[inst.ntype.nib0] <<= 1;
      postvisit(p,0);
      return;
    }

  case 0x9: //9XY0 Skips the next instruction if VX doesn't equal VY.
    if(p->V[inst.ntype.nib0] != p->V[inst.ntype.nib1]) p->pc += 2;
    postvisit(p,0);
    return;

  case 0xA: //ANNN Sets I to the address NNN.
    p->I = inst.jtype.address;
    postvisit(p,0);
    return;

  case 0xB: //BNNN Jumps to the address NNN plus V0.
    p->pc = inst.jtype.address + p->V[0];
    postvisit(p,0);
    return;

  case 0xC: //CNNN Sets VX to a random number and NN.
    p->V[inst.ntype.nib0] = (rand() % 255) & inst.itype.imm;
    postvisit(p,0);
    return;

  case 0xD: //DXYN Draws a sprite at coordinate (VX, VY) ...
    {
      unsigned short x = p->V[inst.ntype.nib0];
      unsigned short y = p->V[inst.ntype.nib1];
      unsigned short height = inst.ntype.nib2;
      unsigned short pixel;

      p->V[0xF] = 0;
      for (int yline = 0; yline < height; yline++) {
	pixel = mem[p->I + yline];
	for (int xline = 0; xline < 8; xline++) {
	  if ((pixel & (0x80 >> xline)) != 0) {
	    if (gfx[x + xline + ((y + yline) * 64)] == 1) p->V[0xF] = 1;
	    gfx[x + xline + ((y + yline) * 64)] ^= 1;
	  }
	}
      }
      p->drawflag = 1;
    }
    postvisit(p,0);
    return;

  case 0xE:
    switch(inst.itype.imm) {
      
    case 0x9E: //EX93 Skips the next instruction if the key stored in VX is pressed.
      if (key[p->V[inst.ntype.nib0]] != 0) p->pc += 2;
      postvisit(p,0);
      return;

    case 0xA1: //EXA1 Skips the next instruction if the key stored in VX isn't pressed.
      if (key[p->V[inst.ntype.nib0]] == 0) p->pc += 2;
      postvisit(p,0);
      return;
    }

  case 0xF:
    switch(inst.itype.imm) {
      
    case 0x07: //FX07 Sets VX to the value of the delay timer.
      p->V[inst.ntype.nib0] = p->delaytimer;
      postvisit(p,0);
      return;

    case 0x0A: //FX0A A key press is awaited, and then stored in VX.
      {
	unsigned char keypress = 0x0;
	for(int i = 0; i < 16; i++) {
	  if(key[i]) {
	    p->V[inst.ntype.nib0] = i;
	    keypress = 0x1;
	  }
	}
	if(!keypress) {
	  postvisit(p,0);
	  return;
	}
      }
      postvisit(p,0);
      return;

    case 0x15: //FX15 Sets the delay timer to VX.
      p->delaytimer = p->V[inst.ntype.nib0];
      postvisit(p,0);
      return;

    case 0x18: //FX18 Sets the sound timer to VX.
      p->soundtimer = p->V[inst.ntype.nib0];
      postvisit(p,0);
      return;

    case 0x1E: //FX1E Adds VX to I
      p->V[0xF] = (p->I + p->V[inst.ntype.nib0] > 0xFFF) ? 1 : 0;
      p->I += p->V[inst.ntype.nib0];
      postvisit(p,0);
      return;

    case 0x29: //FX29 Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
      p->I = p->V[inst.ntype.nib0] * 5;
      postvisit(p,0);
      return;

    case 0x33: //FX33 Stores the Binary-coded decimal representation of VX ...
      mem[p->I] = p->V[inst.ntype.nib0] / 100;
      mem[p->I+1] = (p->V[inst.ntype.nib0] / 10) % 10;
      mem[p->I+2] = (p->V[inst.ntype.nib0] / 100) % 10;
      postvisit(p,0);
      return;

    case 0x55: //FX55 Stores V0 to VX in memory starting at address I.
      for(int i = 0; i < inst.ntype.nib0; i++) mem[p->I + i] = p->V[i];
      p->I += inst.ntype.nib0 + 1;
      postvisit(p,0);
      return;

    case 0x65: //FX65 Fills V0 to VX with values from memory starting at address I.
      for(int i = 0; i < inst.ntype.nib0; i++) p->V[i] = mem[p->I + i];
      p->I += inst.ntype.nib0 + 1;
      postvisit(p,0);
      return;

    case 0x10: //DEBUGGING ONLY Print the first 5 registers
      if (!HARNESS) return;
      printf("%02X", p->V[0]);
      for(int i = 1; i < 5; i++) printf(" %02X", p->V[i]);
      printf("\n");
      exit(0);
      return;
      }

  default:
    printf("Unknown opcode: 0x%04X at 0x%04X\n", inst.bits, p->pc-2);
    exit(0);
    return;
  }
}
