#include "SystemDS.h"
#include "Loader-inl.h"
#include "MatrixOp-inl.h"

#include <glog/logging.h>
#include <iostream>

using namespace std;

namespace ICG {
// Implementation of CoreCGSystem
CoreCGSystem::CoreCGSystem() {
  modelID = Loader::loadObjFromFile("../files/porsche.obj");
  // load KeyFrame
  frameSystem.keyFrames =
      Loader::loadKeyFramesFromFile("../files/keyframes.in");

  interpolater = make_shared<CatmullRomInterpolation>();
  // generate first Frame
  frameSystem.curFrame = interpolater->interpolation(
      frameSystem.keyFrames, frameSystem.curKeyFrame, frameSystem.offsetT);
}
// Implementation of GLUTSystem
shared_ptr<CoreCGSystem> GLUTSystem::cgSystem = nullptr;

void GLUTSystem::init(shared_ptr<CoreCGSystem> cgSystemArg) {
  cgSystem = cgSystemArg;
}
// frameSystem update func
void GLUTSystem::update(void) {
  cgSystem->frameSystem.offsetT += cgSystem->frameSystem.deltaT;
  if (cgSystem->frameSystem.offsetT >= 1) {
    cgSystem->frameSystem.offsetT = 0;
    cgSystem->frameSystem.curKeyFrame++;
  }
  // LOG(ERROR) << curKeyFrame << " " << offsetT;
  if (cgSystem->frameSystem.curKeyFrame + 2 >=
      cgSystem->frameSystem.keyFrames->size()) {
    cgSystem->frameSystem.curKeyFrame = 1;
    return;
  }
  cgSystem->frameSystem.curFrame = cgSystem->interpolater->interpolation(
      cgSystem->frameSystem.keyFrames, cgSystem->frameSystem.curKeyFrame,
      cgSystem->frameSystem.offsetT);
}
// draw model func
void GLUTSystem::drawModel(void) {
  glPushMatrix();

  shared_ptr<TransMatrix> rotationMatrix;
  auto &frame = *(cgSystem->frameSystem.curFrame);

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
  glCallList(cgSystem->modelID);
  glPopMatrix();
}
// callback for dispaly
void GLUTSystem::render(void) {
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
// callback for keyboard
void GLUTSystem::keyboard(unsigned char key, int x, int y) {}
// callback for reshape
void GLUTSystem::reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 1000.0);
  // glOrtho(-25,25,-2,2,0.1,100);
  glMatrixMode(GL_MODELVIEW);
  cgSystem->window.height = h;
  cgSystem->window.width = w;
}
// callback for timer
void GLUTSystem::timer(int value) {
  // increase frame index
  cgSystem->frameSystem.frameCounter++;

  update();

  // render
  glutPostRedisplay();

  // reset timer
  glutTimerFunc(1000.0 / cgSystem->fps, timer, 0);
}

// overload << for debug
ostream &operator<<(ostream &os, const TransMatrix &tMatrix) {
  for (int i = 0; i < 4; ++i) {
    os << "| ";
    for (int j = 0; j < 4; ++j) {
      os << tMatrix.mat[j * 4 + i] << " ";
    }
    os << "|" << endl;
  }
  return os;
}

// overload the << for structs
ostream &operator<<(ostream &os, const Quaternion &quat) {
  os << "(" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z
     << ")";
  return os;
}

ostream &operator<<(ostream &os, const EulerAngles &quat) {
  os << "(" << quat.x << ", " << quat.y << ", " << quat.z << ")";
  return os;
}

ostream &operator<<(ostream &os, const ScalingVec &sVec) {
  os << "(" << sVec.x << ", " << sVec.y << ", " << sVec.z << ")";
  return os;
}

ostream &operator<<(ostream &os, const TranslationVec &tVec) {
  os << "(" << tVec.x << ", " << tVec.y << ", " << tVec.z << ")";
  return os;
}

} // namespace ICG
