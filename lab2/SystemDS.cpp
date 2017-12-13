#include "SystemDS.h"
#include "Loader-inl.h"
#include "MatrixOp-inl.h"

#include <glog/logging.h>
#include <iostream>

using namespace std;

namespace ICG {
// Implementation of CoreCGSystem
void CoreCGSystem::loadDataFromFile(const string &desFile) {
  if (!Loader::loadDesFile(desFile, frameSystem)) {
    LOG(FATAL) << "Failed to read description file: " << desFile;
  }

  // generate first Frame
  for (const auto &object : frameSystem->objects) {
    object->curFrame = object->interpolater->interpolation(
        object->keyFrames, frameSystem->curKeyFrame + object->phase,
        frameSystem->offsetT);
  }
}

// Implementation of GLUTSystem
shared_ptr<CoreCGSystem> GLUTSystem::cgSystem = nullptr;

void GLUTSystem::init(shared_ptr<CoreCGSystem> cgSystemArg) {
  cgSystem = cgSystemArg;
}
// frameSystem update func
void GLUTSystem::update(void) {
  cgSystem->frameSystem->offsetT += cgSystem->frameSystem->deltaT;
  if (cgSystem->frameSystem->offsetT >= 1) {
    cgSystem->frameSystem->offsetT = 0;
    cgSystem->frameSystem->curKeyFrame++;
  }
  // LOG(ERROR) << curKeyFrame << " " << offsetT;
  for (const auto &object : cgSystem->frameSystem->objects) {
    object->curFrame = object->interpolater->interpolation(
        object->keyFrames, cgSystem->frameSystem->curKeyFrame + object->phase,
        cgSystem->frameSystem->offsetT);
  }
}
// draw model func
void GLUTSystem::drawModel(shared_ptr<Object> object) {
  glPushMatrix();

  shared_ptr<TransMatrix> rotationMatrix;
  auto &frame = *(object->curFrame);

  if (frame.orientationType == ICG_QUATERNION) {
    rotationMatrix = make_shared<TransMatrix>(
        *static_pointer_cast<Quaternion>(frame.orientation));
  } else if (frame.orientationType == ICG_EULER) {
    rotationMatrix = make_shared<TransMatrix>(
        *static_pointer_cast<EulerAngles>(frame.orientation));
  }

  TransMatrix scalingMatrix(*(frame.scalingVec));
  TransMatrix tranlationMatrix(*(frame.translationVec));

  auto jointVec = TranslationVec(object->joint);
  TransMatrix jointMatrix(jointVec);

  // jointMatrix * tranlationMatrix * rotationMatrix * scalingMatrix * point
  glMultMatrixd(&(jointMatrix.mat[0]));
  glMultMatrixd(&(tranlationMatrix.mat[0]));
  glMultMatrixd(&(rotationMatrix->mat[0]));
  glMultMatrixd(&(scalingMatrix.mat[0]));

  // glColor3f(1.0, 0.23, 0.27);
  glCallList(object->modelID);
  // draw successor by joint
  if (object->sons.size()) {
    for (const auto &son : object->sons) {
      drawModel(son);
    }
  }
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
  for (const auto &object : cgSystem->frameSystem->objects) {
    if (object->parent == nullptr) {
      drawModel(object);
    }
  }

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
  cgSystem->window->height = h;
  cgSystem->window->width = w;
}
// callback for timer
void GLUTSystem::timer(int value) {
  // increase frame index
  cgSystem->frameSystem->frameCounter++;

  update();

  // render
  glutPostRedisplay();

  // reset timer
  glutTimerFunc(1000.0 / cgSystem->frameSystem->fps, timer, 0);
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
