#include "chip8.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


void cpucycle() {
  inst = fetch();
  switch(inst.ntype.opcode) {

  case 0x0:
    switch(inst.ntype.nib2) {

    case 0x0: //00E0: Clear Screen
      for(int i = 0; i < 2048; i++) gfx[i] = 0;
      drawflag = 1;
      break;

    case 0xE: //00EE: Return from subroutine
      --sp;
      pc = stack[sp];
      pc += 2;
      break;
    }

  case 0x1: //1NNN Jump to address NNN
    pc = inst.jtype.address;
    break;

  case 0x2: //2NNN Call subroutine at NNN
    stack[sp] = pc - 2;
    ++sp;
    pc = inst.jtype.address;
    break;

  case 0x3: //3XNN Skips the next instruction if VX equals NN.
    if (V[inst.ntype.nib0] == inst.itype.imm) pc += 2;
    break;

  case 0x4: //4XNN Skips the next instruction if VX doesn't equal NN.
    if (V[inst.ntype.nib0] != inst.itype.imm) pc += 2;
    break;

  case 0x5: //5XY0 Skips the next instruction if VX equals VY.
    if (V[inst.ntype.nib0] == V[inst.ntype.nib1]) pc += 2;
    break;

  case 0x6: //6XNN Sets VX to NN.
    V[inst.ntype.nib0] = inst.itype.imm;
    break;

  case 0x7: //7XNN Adds NN to VX.
    V[inst.ntype.nib0] += inst.itype.imm;
    break;

  case 0x8: //Arithmetic things
    switch(inst.ntype.nib2) {
      
    case 0x0: //8XY0 Sets VX to the value of VY.
      V[inst.ntype.nib0] = V[inst.ntype.nib1];
      break;

    case 0x1: //8XY1 Sets VX to VX or VY.
      V[inst.ntype.nib0] = V[inst.ntype.nib0] | V[inst.ntype.nib1];
      break;
      
    case 0x2: //8XY2 Sets VX to VX and VY.
      V[inst.ntype.nib0] = V[inst.ntype.nib0] & V[inst.ntype.nib1];
      break;

    case 0x3: //8XY3 Sets VX to VX xor VY.
      V[inst.ntype.nib0] = V[inst.ntype.nib0] ^ V[inst.ntype.nib1];
      break;
    
    case 0x4: //8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
      V[0xF] = (V[inst.ntype.nib1] > (0xFF - V[inst.ntype.nib0])) ? 1 : 0;
      V[inst.ntype.nib0] += V[inst.ntype.nib1];
      break;

    case 0x5: //8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      V[0xF] = (V[inst.ntype.nib0] > V[inst.ntype.nib1]) ? 1 : 0;
      V[inst.ntype.nib0] = V[inst.ntype.nib0] - V[inst.ntype.nib1];
      break;
      
    case 0x6: //8XY6 Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
      V[0xF] = V[inst.ntype.nib0] & 1;
      V[inst.ntype.nib0] >>= 1;
      break;

    case 0x7: //8XY7 Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      V[0xF] = (V[inst.ntype.nib1] > V[inst.ntype.nib0]) ? 1 : 0;
      V[inst.ntype.nib0] = V[inst.ntype.nib1] - V[inst.ntype.nib0];
      break;

    case 0xE: //8XYE Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
      V[0xF] = V[inst.ntype.nib0] >> 7;
      V[inst.ntype.nib0] <<= 1;
      break;
    }

  case 0x9: //9XY0 Skips the next instruction if VX doesn't equal VY.
    if(V[inst.ntype.nib0] != V[inst.ntype.nib1]) pc += 2;
    break;

  case 0xA: //ANNN Sets I to the address NNN.
    I = inst.jtype.address;
    break;

  case 0xB: //BNNN Jumps to the address NNN plus V0.
    pc = inst.jtype.address + V[0];
    break;

  case 0xC: //CNNN Sets VX to a random number and NN.
    V[inst.ntype.nib0] = (rand() % 255) & inst.itype.imm;
    break;

  case 0xD: //DXYN Draws a sprite at coordinate (VX, VY) ...
    {
      unsigned short x = V[inst.ntype.nib0];
      unsigned short y = V[inst.ntype.nib1];
      unsigned short height = inst.ntype.nib2;
      unsigned short pixel;

      V[0xF] = 0;
      for (int yline = 0; yline < height; yline++) {
	pixel = mem[I + yline];
	for (int xline = 0; xline < 8; xline++) {
	  if ((pixel & (0x80 >> xline)) != 0) {
	    if (gfx[x + xline + ((y + yline) * 64)] == 1) V[0xF] = 1;
	    gfx[x + xline + ((y + yline) * 64)] ^= 1;
	  }
	}
      }
      drawflag = 1;
    }
    break;

  case 0xE:
    switch(inst.itype.imm) {
      
    case 0x93: //EX93 Skips the next instruction if the key stored in VX is pressed.
      if (key[V[inst.ntype.nib0]] != 0) pc += 2;
      break;

    case 0xA1: //EXA1 Skips the next instruction if the key stored in VX isn't pressed.
      if (key[V[inst.ntype.nib0]] == 0) pc += 2;      
      break;
    }

  case 0xF:
    switch(inst.itype.imm) {
      
    case 0x07: //FX07 Sets VX to the value of the delay timer.
      V[inst.ntype.nib0] = delaytimer;
      break;

    case 0x0A: //FX0A A key press is awaited, and then stored in VX.
      {
	unsigned char keypress = 0x0;
	for(int i = 0; i < 16; i++) {
	  if(key[i]) {
	    V[inst.ntype.nib0] = i;
	    keypress = 0x1;
	  }
	}
	if(!keypress) {
	  pc -= 2;
	  return;
	}
      }
      break;

    case 0x15: //FX15 Sets the delay timer to VX.
      delaytimer = V[inst.ntype.nib0];
      break;

    case 0x18: //FX18 Sets the sound timer to VX.
      soundtimer = V[inst.ntype.nib0];
      break;

    case 0x1E: //FX1E Adds VX to I
      V[0xF] = (I + V[inst.ntype.nib0] > 0xFFF) ? 1 : 0;
      I += V[inst.ntype.nib0];
      break;

    case 0x29: //FX29 Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
      I = V[inst.ntype.nib0] * 5;
      break;

    case 0x33: //FX33 Stores the Binary-coded decimal representation of VX ...
      mem[I] = V[inst.ntype.nib0] / 100;
      mem[I+1] = (V[inst.ntype.nib0] / 10) % 10;
      mem[I+2] = (V[inst.ntype.nib0] / 100) % 10;
      break;

    case 0x55: //FX55 Stores V0 to VX in memory starting at address I.
      for(int i = 0; i < inst.ntype.nib0; i++) mem[I + i] = V[i];
      I += inst.ntype.nib0 + 1;
      break;

    case 0x65: //FX65 Fills V0 to VX with values from memory starting at address I.
      for(int i = 0; i < inst.ntype.nib0; i++) V[i] = mem[I + i];
      I += inst.ntype.nib0 + 1;
      break;
    }

  default:
    printf("Unknown opcode: 0x%04X\n", inst.bits);
    break;
  }

  if (delaytimer) --delaytimer;
 
  if(soundtimer){
    if(soundtimer == 1) printf("BEEP!\n");
    --soundtimer;
  }  
}
