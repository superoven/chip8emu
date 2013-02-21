#include <stdio.h>
#include <GL/glut.h>
#include "chip8.h"

int HARNESS;
int display_width;
int display_height;
processor_t p;

void emulationLoop() {
  cpucycle(&p);
  if ((&p)->drawflag) {
    glClear(GL_COLOR_BUFFER_BIT);
    drawScreen();
    (&p)->drawflag = 0;
  }
}

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

  char* filename;
  
  if (argc == 3 && (int)argv[1][0] == '-' && (int)argv[1][1] == 'h') {
    HARNESS = 1;
    filename = argv[2];
  }
  else {
    HARNESS = 0;
    filename = argv[1];
  }

  int filesize = loadrom(filename, &romdata);
  display_width = WIDTH*MODIFIER;
  display_height = HEIGHT*MODIFIER;

  initialize(&p, filesize);

  if (DEBUGMODE) step(&p);

  //while(1) display(&p);
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  glutInitWindowSize(display_width, display_height);
  glutInitWindowPosition(320, 160);
  glutCreateWindow("Chip-8 Emulator");

  glutDisplayFunc(emulationLoop);
  glutIdleFunc(emulationLoop);
  glutReshapeFunc(reshapeWindow);
  glutKeyboardFunc(keyboardDown);
  glutKeyboardUpFunc(keyboardUp);

  glutMainLoop();

  return 0;
}
