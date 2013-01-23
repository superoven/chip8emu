#include "chip8.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

processor_t p;

void disassemble(inst_t a) {

  inst.bits = a.bits;

  switch(inst.ntype.opcode) {

    case 0x0:
    switch(inst.ntype.nib2) {

    case 0x0: //00E0: Clear Screen
      printf("%s\n", "CLEAR");
      break;

    case 0xE: //00EE: Return from subroutine
      printf("%s\n", "RETURN");
      return;
    }

  case 0x1: //1NNN Jump to address NNN
    printf("%s %04X\n", "JUMPTO", inst.jtype.address);
    return;

  case 0x2: //2NNN Call subroutine at NNN
    printf("%s %04X\n", "CALL", inst.jtype.address);
    return;

  case 0x3: //3XNN Skips the next instruction if VX equals NN.
    printf("%s V%d = %04X\n", "SKIPIF", inst.ntype.nib0, inst.itype.imm);
    return;

  case 0x4: //4XNN Skips the next instruction if VX doesn't equal NN.
    printf("%s V%d != %04X\n", "SKIPIF", inst.ntype.nib0, inst.itype.imm);
    return;

  case 0x5: //5XY0 Skips the next instruction if VX equals VY.
    printf("%s V%d != V%d\n", "SKIPIF", inst.ntype.nib0, inst.ntype.nib1);
    return;

  case 0x6: //6XNN Sets VX to NN.
    printf("%s V%d %04X\n", "SET", inst.ntype.nib0, inst.itype.imm);
    return;

  case 0x7: //7XNN Adds NN to VX.
    printf("%s %04X V%d\n", "ADDTO", inst.itype.imm, inst.ntype.nib0);
    return;

  case 0x8: //Arithmetic things
    switch(inst.ntype.nib2) {
      
    case 0x0: //8XY0 Sets VX to the value of VY.
      printf("%s V%d to V%d\n", "SET", inst.ntype.nib0, inst.ntype.nib1);
      return;

    case 0x1: //8XY1 Sets VX to VX or VY.
      printf("%s V%d V%d\n", "SETOR", inst.ntype.nib0, inst.ntype.nib1);
      return;
      
    case 0x2: //8XY2 Sets VX to VX and VY.
      printf("%s V%d V%d\n", "SETAND", inst.ntype.nib0, inst.ntype.nib1);
      return;

    case 0x3: //8XY3 Sets VX to VX xor VY.
      printf("%s V%d V%d\n", "SETXOR", inst.ntype.nib0, inst.ntype.nib1);
      return;
    
    case 0x4: //8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
      printf("%s V%d to V%d\n", "ADD", inst.ntype.nib1, inst.ntype.nib0);
      return;

    case 0x5: //8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      printf("%s V%d from V%d\n", "SUB", inst.ntype.nib1, inst.ntype.nib0);
      return;
      
    case 0x6: //8XY6 Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
      printf("%s V%d\n", "SR", inst.ntype.nib0);
      return;

    case 0x7: //8XY7 Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      printf("%s V%d minus V%d\n", "SUBSETVX", inst.ntype.nib1, inst.ntype.nib0);
      return;

    case 0xE: //8XYE Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
      printf("%s V%d\n", "SL", inst.ntype.nib0);
      return;
    }

  case 0x9: //9XY0 Skips the next instruction if VX doesn't equal VY.
    printf("%s V%d != V%d\n", "SKIPIF", inst.ntype.nib0, inst.ntype.nib1);
    return;

  case 0xA: //ANNN Sets I to the address NNN.
    printf("%s %04X\n", "SETI", inst.jtype.address);
    return;

  case 0xB: //BNNN Jumps to the address NNN plus V0.
    printf("%s %04X\n", "JUMP+V0", inst.jtype.address);
    return;

  case 0xC: //CNNN Sets VX to a random number and NN.
    printf("%s V%d %04X\n", "RANDOM", inst.ntype.nib0, inst.itype.imm);
    return;

  case 0xD: //DXYN Draws a sprite at coordinate (VX, VY) ...
    printf("%s V%d V%d\n", "DRAW", inst.ntype.nib0, inst.ntype.nib1);
    return;

  case 0xE:
    switch(inst.itype.imm) {
      
    case 0x93: //EX93 Skips the next instruction if the key stored in VX is pressed.
      printf("%s V%d\n", "SKIPIFKEY", inst.ntype.nib0);
      return;

    case 0xA1: //EXA1 Skips the next instruction if the key stored in VX isn't pressed.
      printf("%s V%d\n", "SKIPIFNOTKEY", inst.ntype.nib0);
      return;
    }

  case 0xF:
    switch(inst.itype.imm) {
      
    case 0x07: //FX07 Sets VX to the value of the delay timer.
      printf("%s V%d\n", "SETVXTODELAY", inst.ntype.nib0);
      return;

    case 0x0A: //FX0A A key press is awaited, and then stored in VX.
      printf("%s V%d\n", "WAITFORKEY", inst.ntype.nib0);
      return;

    case 0x15: //FX15 Sets the delay timer to VX.
      printf("%s V%d\n", "SETDELAYTO", inst.ntype.nib0);
      return;

    case 0x18: //FX18 Sets the sound timer to VX.
      printf("%s V%d\n", "SETSOUNDTO", inst.ntype.nib0);
      return;

    case 0x1E: //FX1E Adds VX to I
      printf("%s V%d\n", "ADDTOI", inst.ntype.nib0);
      return;

    case 0x29: //FX29 Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
      printf("%s V%d\n", "IJUMP", inst.ntype.nib0);
      return;

    case 0x33: //FX33 Stores the Binary-coded decimal representation of VX ...
      printf("%s V%d\n", "BINDEC", inst.ntype.nib0);
      return;

    case 0x55: //FX55 Stores V0 to VX in memory starting at address I.
      printf("%s until V%d\n", "REGTOMEM", inst.ntype.nib0);
      return;

    case 0x65: //FX65 Fills V0 to VX with values from memory starting at address I.
      printf("%s until V%d\n", "MEMTOREG", inst.ntype.nib0);
      return;
      }

  default:
    printf("Unknown opcode: 0x%04X at 0x%04X\n", inst.bits, p.pc-2);
    return;
  }
}
