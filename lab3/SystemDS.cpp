#include "SystemDS.h"
#include "Loader-inl.h"
#include "MatrixOp-inl.h"

#include <cmath>
#include <glog/logging.h>
#include <iostream>

using namespace std;

namespace ICG {
const double Object::g = 9.8;
const double Object::eps = 1e-3;
void Object::calFrame() {
  vector<double> initVec{pos[0],      pos[1],      pos[2],
                         rotation[0], rotation[1], rotation[2]};
  curFrame = make_shared<Frame>(initVec, false);
};

void Object::boxCheck(double boxSize) {
  for (int i = 0; i < 3; ++i) {
    double posCur = pos[i];
    if (i == 2) {
      posCur += 3 * boxSize;
    }
    if (posCur - radius <= -boxSize) {
      v[i] = abs(v[i]) * cofRes;
    }
    if (posCur + radius >= boxSize) {
      v[i] = -abs(v[i]) * cofRes;
    }
    if (abs(v[i]) < eps) {
      v[i] = 0;
    }
  }
}

void Object::calPos(const double &deltaT, double boxSize) {
  bool hitGround = abs(pos[1] - radius + boxSize) < eps;
  bool touchGround = abs(pos[1] - radius + boxSize) < 1e-1;
  for (int i = 0; i < 3; ++i) {
    double newV = v[i];
    if (i == 1) {
      if (!hitGround) {
        newV = v[1] - g * deltaT;
      }
    }
    if (touchGround && i != 1) {
      double vprime = abs(v[i]) - g * friction * deltaT;
      if (vprime > 0) {
        newV = (v[i] / abs(v[i])) * vprime;
      } else {
        newV = 0;
      }
    }
    pos[i] += (newV + v[i]) / 2 * deltaT;
    v[i] = newV;

    rotation[i] += av[i];
    while (rotation[i] >= 360) rotation[i] -= 360;
    while (rotation[i] < 0) rotation[i] += 360;
    if (touchGround) {
      av[i] = av[i] * friction;
      if (av[i] <= eps) {
        av[i] = 0;
      }
    }
  }
  boxCheck(boxSize);
};

static double distance(shared_ptr<Object> a, shared_ptr<Object> b) {
  double sum = 0;
  for (int i = 0; i < 3; ++i) {
    sum += (a->pos[i] - b->pos[i]) * (a->pos[i] - b->pos[i]);
  }
  return sqrt(sum);
}
void FrameSystem::collisionCheck() {
  int cnt = objects.size();
  for (int i = 0; i < cnt; ++i) {
    for (int j = i + 1; j < cnt; ++j) {
      if (distance(objects[i], objects[j]) + 1e-3 <=
          (objects[i]->radius + objects[j]->radius)) {
        LOG(ERROR) << distance(objects[i], objects[j]);
        for (int k = 0; k < 3; ++k) {
          double moSum = objects[i]->mass * objects[i]->v[k] +
                         objects[j]->mass * objects[j]->v[k];
          double vi = (moSum + objects[i]->cofRes * objects[j]->mass *
                                   (objects[j]->v[k] - objects[i]->v[k])) /
                      (objects[i]->mass + objects[j]->mass);
          double vj = (moSum + objects[j]->cofRes * objects[i]->mass *
                                   (objects[i]->v[k] - objects[j]->v[k])) /
                      (objects[i]->mass + objects[j]->mass);
          LOG(ERROR) << objects[i]->v[k] << " " << objects[j]->v[k];
          LOG(ERROR) << vi << " " << vj;
          objects[i]->v[k] = vi;
          objects[j]->v[k] = vj;

          objects[i]->av[k] = vi;
          objects[j]->av[k] = vj;
        }
      }
    }
  }
  return;
}

// Implementation of CoreCGSystem
void CoreCGSystem::loadDataFromFile(const string &desFile) {
  if (!Loader::loadDesFile(desFile, frameSystem)) {
    LOG(FATAL) << "Failed to read description file: " << desFile;
  }
}

// Implementation of GLUTSystem
shared_ptr<CoreCGSystem> GLUTSystem::cgSystem = nullptr;

void GLUTSystem::init(shared_ptr<CoreCGSystem> cgSystemArg) {
  cgSystem = cgSystemArg;
}

// frameSystem update func
void GLUTSystem::update(void) {
  cgSystem->frameSystem->frameCounter++;
  for (const auto &object : cgSystem->frameSystem->objects) {
    object->calPos(cgSystem->frameSystem->deltaT,
                   cgSystem->frameSystem->boxSize);
    object->calFrame();
  }
  cgSystem->frameSystem->collisionCheck();
}
// draw model func
void GLUTSystem::drawModel(shared_ptr<Object> object, bool trans) {
  glPushMatrix();
  if (trans) {
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

    // tranlationMatrix * rotationMatrix * scalingMatrix * point
    glMultMatrixd(&(tranlationMatrix.mat[0]));
    glMultMatrixd(&(rotationMatrix->mat[0]));
    glMultMatrixd(&(scalingMatrix.mat[0]));
  }

  glColor3f(0, 0, 0);
  glCallList(object->modelID);

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
  drawModel(cgSystem->frameSystem->boxObj);
  // render objects
  for (const auto &object : cgSystem->frameSystem->objects) {
    drawModel(object);
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
  // glOrtho(-25,25,-25,25,100,100);
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
