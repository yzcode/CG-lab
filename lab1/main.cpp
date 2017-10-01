// custom lib
#include "Loader-inl.h"
#include "MatrixOp-inl.h"
#include "interpolation-inl.h"
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

// pointer to interpolater
shared_ptr<BaseInterpolation> interpolater;
// model id
GLuint modelID = 0;
shared_ptr<vector<Frame>> keyFrames;
shared_ptr<Frame> curFrame;

double deltaT = 0.01;
double offsetT = 0;
int curKeyFrame = 0;
const int fps = 60;
//================================
// init
//================================
void init(void) {
  // load objFile
  modelID = Loader::loadObjFromFile("../files/porsche.obj");
  // load KeyFrame
  keyFrames = Loader::loadKeyFramesFromFile("../files/keyframes.in");

  interpolater = make_shared<LineInterpolation>();
  // generate first Frame
  curFrame = interpolater->interpolation(
      keyFrames->at(curKeyFrame), keyFrames->at(curKeyFrame + 1), offsetT);
}

//================================
// update
//================================
void update(void) {
  offsetT += deltaT;
  if (offsetT >= 1) {
    offsetT = 0;
    curKeyFrame++;
  }
  // LOG(ERROR) << curKeyFrame << " " << offsetT;
  if (curKeyFrame + 1 >= keyFrames->size()) {
    curKeyFrame = 0;
    return;
  }
  curFrame = interpolater->interpolation(
      keyFrames->at(curKeyFrame), keyFrames->at(curKeyFrame + 1), offsetT);

}

void drawModel() {
  glPushMatrix();

  shared_ptr<TransMatrix> rotationMatrix;
  auto &frame = *curFrame;

  if (frame.orientationType == ICG_QUATERNION) {
    rotationMatrix = make_shared<TransMatrix>(
        *static_pointer_cast<Quaternion>(frame.orientation));
  } else if (frame.orientationType == ICG_EULER) {
    rotationMatrix = make_shared<TransMatrix>(
        *static_pointer_cast<EulerAngles>(frame.orientation));
  }

  TransMatrix scalingMatrix(*(frame.scalingVec));
  TransMatrix tranlationMatrix(*(frame.translationVec));

  // unit-matrix * tranlationMatrix * rotationMatrix * scalingMatrix * point
  glMultMatrixd(&(tranlationMatrix.mat[0]));
  glMultMatrixd(&(rotationMatrix->mat[0]));
  glMultMatrixd(&(scalingMatrix.mat[0]));

  glColor3f(1.0, 0.23, 0.27);
  glCallList(modelID);
  glPopMatrix();
}

//================================
// render
//================================
void render(void) {
  // clear buffer
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClearDepth(1.0);
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
void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 1000.0);
  // glOrtho(-25,25,-2,2,0.1,100);
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
  // main loop
  glutMainLoop();

  return 0;
}
