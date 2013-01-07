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
      
      
