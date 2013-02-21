#include <stdio.h>
#include <GL/glut.h>
#include "chip8.h"

void step(processor_t* p) {
  cpucycle(p);
  getchar();
  step(p);
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage: ./chipemu [rom]\n");
    return 0;
  }

  processor_t p;

  int filesize = loadrom(argv[1], &romdata);
  display_width = WIDTH*MULT;
  display_height = HEIGHT*MULT;

  initialize(&p, filesize);

  //step(&p);

  while (1) display(&p);
  
  /*glutInit(&argc, argv);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0, display_width, display_height, 0, 0, 1);
  glMatrixMode (GL_MODELVIEW);
  glDisable(GL_DEPTH_TEST);

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
