#include "chip8.h"
#include <stdio.h>

void cpucycle() {
  inst = fetch();
  switch(inst.ntype.opcode) {

  case 0x0:
    switch(inst.ntype.nib2) {

    case 0x0: //00E0: Clear Screen
      //Do it
      break;

    case 0xE: //00EE: Return from subroutine
      //Do it
      break;

    default:
      printf("Unknown opcode: 0x%04X\n", inst.bits);
      break;
    }

  case 0x1: //1NNN Jump to address NNN
    //Do it
    break;

  case 0x2: //2NNN Call subroutine at NNN
    //Do it
    break;

  case 0x3: //3XNN Skips the next instruction if VX equals NN.
    //Do it
    break;

  case 0x4: //4XNN Skips the next instruction if VX doesn't equal NN.
    //Do it
    break;

  case 0x5: //5XNN Skips the next instruction if VX equals VY.
    //Do it
    break;

  case 0x6: //6XNN Sets VX to NN.
    //Do it
    break;

  case 0x7: //7XNN Adds NN to VX.
    //Do it
    break;

  case 0x8: //Arithmetic things
    switch(inst.ntype.nib2) {
      
    case 0x0: //8XY0 Sets VX to the value of VY.
      //Do it
      break;

    case 0x1: //8XY1 Sets VX to VX or VY.
      //Do it
      break;
      
    case 0x2: //8XY2 Sets VX to VX and VY.
      //Do it
      break;

    case 0x3: //8XY3 Sets VX to VX xor VY.
      //Do it
      break;
    
    case 0x4: //8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
      //Do it
      break;

    case 0x5: //8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      //Do it
      break;
      
    case 0x6: //8XY6 Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
      //Do it
      break;

    case 0x7: //8XY7 Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
      //Do it
      break;

    case 0xE: //8XYE Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
      //Do it
      break;
    }

  case 0x9: //9XY0 Skips the next instruction if VX doesn't equal VY.
    //Do it
    break;

  case 0xA: //ANNN Sets I to the address NNN.
    //Do it
    break;

  case 0xB: //BNNN Jumps to the address NNN plus V0.
    //Do it
    break;

  case 0xC: //CNNN Sets VX to a random number and NN.
    //Do it
    break;

  case 0xD: //DXYN Draws a sprite at coordinate (VX, VY) ...
    //Do it
    break;

    

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
