#include "chip8.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void disassemble(inst_t a, FILE* fp) {

  inst.bits = a.bits;

  switch(inst.ntype.opcode) {

    case 0x0:
      if (inst.bits == 0x00E0) {
	fprintf(fp,"%s\n", "CLS");
	return;
      }
      if (inst.bits == 0x00EE) {
	fprintf(fp,"%s\n", "RET");
	return;
      }
      fprintf(fp,"DW #%04X\n", inst.bits);
      return;

  case 0x1: //1NNN Jump to address NNN
    fprintf(fp,"%s #%03X\n", "JP", inst.jtype.address >> 1);
    return;

  case 0x2: //2NNN Call subroutine at NNN
    fprintf(fp,"%s #%03X\n", "CALL", inst.jtype.address >> 1);
    return;

  case 0x3: //3XNN Skips the next instruction if VX equals NN.
    fprintf(fp,"%s V%X, #%02X\n", "SE", inst.ntype.nib0, inst.itype.imm);
    return;

  case 0x4: //4XNN Skips the next instruction if VX doesn't equal NN.
    fprintf(fp,"%s V%X, #%02X\n", "SNE", inst.ntype.nib0, inst.itype.imm);
    return;

  case 0x5: //5XY0 Skips the next instruction if VX equals VY.
    if (inst.ntype.nib2) {
      fprintf(fp,"DW #%04X\n", inst.bits);
      return;
    }
    fprintf(fp,"%s V%X, V%X\n", "SE", inst.ntype.nib0, inst.ntype.nib1);
    return;

  case 0x6: //6XNN Sets VX to NN.
    fprintf(fp,"%s V%X, #%02X\n", "LD", inst.ntype.nib0, inst.itype.imm);
    return;

  case 0x7: //7XNN Adds NN to VX.
    fprintf(fp,"%s V%X, #%02X\n", "ADD", inst.ntype.nib0, inst.itype.imm);
    return;

  case 0x8: //Arithmetic things
    switch(inst.ntype.nib2) {
      
    case 0x0: //8XY0 Sets VX to the value of VY.
      if (inst.ntype.nib2) {
	fprintf(fp,"DW #%04X\n", inst.bits);
	return;
      }
      fprintf(fp,"%s V%X, V%X\n", "LD", inst.ntype.nib0, inst.ntype.nib1);
      return;

    case 0x1: //8XY1 Sets VX to VX or VY.
      if (inst.ntype.nib2 != 1) {
	fprintf(fp,"DW #%04X\n", inst.bits);
	return;
      }
      fprintf(fp,"%s V%X, V%X\n", "OR", inst.ntype.nib0, inst.ntype.nib1);
      return;
      
    case 0x2: //8XY2 Sets VX to VX and VY.
      if (inst.ntype.nib2 != 2) {
	fprintf(fp,"DW #%04X\n", inst.bits);
	return;
      }
      fprintf(fp,"%s V%X, V%X\n", "AND", inst.ntype.nib0, inst.ntype.nib1);
      return;

    case 0x3: //8XY3 Sets VX to VX xor VY.
      if (inst.ntype.nib2 != 3) {
	fprintf(fp,"DW #%04X\n", inst.bits);
	return;
      }
      fprintf(fp,"%s V%X, V%X\n", "XOR", inst.ntype.nib0, inst.ntype.nib1);
      return;
    
    case 0x4: //8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
      fprintf(fp,"%s V%X, V%X\n", "ADD", inst.ntype.nib0, inst.ntype.nib1);
      return;

    case 0x5: //8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      fprintf(fp,"%s V%X, V%X\n", "SUB", inst.ntype.nib0, inst.ntype.nib1);
      return;
      
    case 0x6: //8XY6 Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
      fprintf(fp,"%s V%X, V%X\n", "SHR", inst.ntype.nib0, inst.ntype.nib1);
      return;

    case 0x7: //8XY7 Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      fprintf(fp,"%s V%X, V%X\n", "SUBN", inst.ntype.nib0, inst.ntype.nib1);
      return;

    case 0xE: //8XYE Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
      fprintf(fp,"%s V%X, V%X\n", "SHL", inst.ntype.nib0, inst.ntype.nib1);
      return;
    }

  case 0x9: //9XY0 Skips the next instruction if VX doesn't equal VY.
    if (inst.ntype.nib2) {
      fprintf(fp,"DW #%04X\n", inst.bits);
      return;
    }
    fprintf(fp,"%s V%X, V%X\n", "SNE", inst.ntype.nib0, inst.ntype.nib1);
    return;

  case 0xA: //ANNN Sets I to the address NNN.
    fprintf(fp,"%s I, #%03X\n", "LD", inst.jtype.address >> 1);
    return;

  case 0xB: //BNNN Jumps to the address NNN plus V0.
    fprintf(fp,"%s V0, #%03X\n", "JP", inst.jtype.address);
    return;

  case 0xC: //CNNN Sets VX to a random number and NN.
    fprintf(fp,"%s V%X, #%02X\n", "RND", inst.ntype.nib0, inst.itype.imm);
    return;

  case 0xD: //DXYN Draws a sprite at coordinate (VX, VY) ...
    fprintf(fp,"%s V%X, V%X, #%X\n", "DRW", inst.ntype.nib0, inst.ntype.nib1, inst.ntype.nib2);
    return;

  case 0xE:
    switch(inst.itype.imm) {
      
    case 0x9E: //EX93 Skips the next instruction if the key stored in VX is pressed.
      fprintf(fp,"%s V%X\n", "SKP", inst.ntype.nib0);
      return;

    case 0xA1: //EXA1 Skips the next instruction if the key stored in VX isn't pressed.
      fprintf(fp,"%s V%X\n", "SKNP", inst.ntype.nib0);
      return;
    }

  case 0xF:
    switch(inst.itype.imm) {
      
    case 0x07: //FX07 Sets VX to the value of the delay timer.
      fprintf(fp,"%s V%X, DT\n", "LD", inst.ntype.nib0);
      return;

    case 0x0A: //FX0A A key press is awaited, and then stored in VX.
      fprintf(fp,"%s V%X, K\n", "LD", inst.ntype.nib0);
      return;

    case 0x15: //FX15 Sets the delay timer to VX.
      fprintf(fp,"%s DT, V%X\n", "LD", inst.ntype.nib0);
      return;

    case 0x18: //FX18 Sets the sound timer to VX.
      fprintf(fp,"%s ST, V%X\n", "LD", inst.ntype.nib0);
      return;

    case 0x1E: //FX1E Adds VX to I
      fprintf(fp,"%s I, V%X\n", "ADD", inst.ntype.nib0);
      return;

    case 0x29: //FX29 Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
      fprintf(fp,"%s F, V%X\n", "LD", inst.ntype.nib0);
      return;

    case 0x33: //FX33 Stores the Binary-coded decimal representation of VX ...
      fprintf(fp,"%s B, V%X\n", "LD", inst.ntype.nib0);
      return;

    case 0x55: //FX55 Stores V0 to VX in memory starting at address I.
      fprintf(fp,"%s [I], V%X\n", "LD", inst.ntype.nib0);
      return;

    case 0x65: //FX65 Fills V0 to VX with values from memory starting at address I.
      fprintf(fp,"%s V%X, [I]\n", "LD", inst.ntype.nib0);
      return;
      }

  default:
    fprintf(fp,"DW #%04X\n", inst.bits);
    return;
  }
}
