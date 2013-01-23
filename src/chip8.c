#include <stdio.h>
#include <GL/glut.h>
#include "chip8.h"

processor_t p;

void step() {
  cpucycle();
  getchar();
  step();
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage: ./chipemu [rom]\n");
    return 0;
  }

  int filesize = loadrom(argv[1], &romdata);
  display_width = WIDTH*MULT;
  display_height = HEIGHT*MULT;

  initialize(&p, filesize);

  step();
  
  /*glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA );
  glutInitWindowPosition(320,160);
  glutInitWindowSize(display_width, display_height);
  glutCreateWindow("CHIP-8 Emulator");

  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutKeyboardFunc(keyboardDown);
  glutKeyboardUpFunc(keyboardUp); 

  glutMainLoop();*/

  return 0;
}
