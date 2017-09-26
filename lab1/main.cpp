// custom lib
#include "ObjLoader-inl.h"

// standard
#include <cassert>
#include <cmath>
#include <iostream>

// glut
#include <GLUT/glut.h>

using namespace ICG;
//================================
// global variables
//================================
// screen size
int g_screenWidth = 0;
int g_screenHeight = 0;

// frame index
int g_frameIndex = 0;

// model id
GLuint modelID = 0;
// angle for rotation
double g_angle = 0;
constexpr double speed = 1;
constexpr int fps = 72;
constexpr double rotationOffset = (360 * speed) / fps;
//================================
// init
//================================
void init(void) {
  // init something before main loop...
}

//================================
// update
//================================
void update(void) {
  // do something before rendering...

  // rotation angle
  g_angle = (g_angle + rotationOffset);
  while (g_angle >= 360) {
    g_angle -= 360;
  }
}

void drawModel()
{
    glPushMatrix();
    glTranslatef(0,0,-105);
    glColor3f(1.0,0.23,0.27);
    glScalef(1,1,1);
    glRotatef(g_angle,0,1,0);
    glCallList(modelID);
    glPopMatrix();
}

//================================
// render
//================================
void render(void) {
  // clear buffer
  glClearColor(0.0, 0.0, 0.0, 0.0);
  // glClearDepth(1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render state
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  glLoadIdentity();
  // render objects
  drawModel();

  // swap back and front buffers
  glutSwapBuffers();
}

//================================
// keyboard input
//================================
void keyboard(unsigned char key, int x, int y) {}

//================================
// reshape : update viewport and projection matrix when the window is resized
//================================
void reshape(int w,int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 0.1, 1000.0);
    //glOrtho(-25,25,-2,2,0.1,100);
    glMatrixMode(GL_MODELVIEW);
}

//================================
// timer : triggered every 16ms ( about 60 frames per second )
//================================
void timer(int value) {
  // increase frame index
  g_frameIndex++;

  update();

  // render
  glutPostRedisplay();

  // reset timer
  glutTimerFunc(1000.0 / fps, timer, 0);
}

//================================
// main
//================================
int main(int argc, char **argv) {
  // init glog
  google::InitGoogleLogging(argv[0]);

  // create opengL window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);

  // init
  init();

  // set callback functions
  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutTimerFunc(1000.0 / fps, timer, 0);

  // load objFile
  modelID = ObjLoader::loadObjFromFile("../files/porsche.obj");
  // main loop
  glutMainLoop();

  return 0;
}
