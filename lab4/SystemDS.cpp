#include "SystemDS.h"
#include "Loader-inl.h"
#include "MatrixOp-inl.h"

#include <cmath>
#include <cstdlib>
#include <glog/logging.h>
#include <iostream>

using namespace std;

namespace ICG {
void Object::calFrame() {
  vector<double> initVec{pos[0], pos[1], pos[2], 0, 0, 0};
  curFrame = make_shared<Frame>(initVec, false);
};

void Object::calPos(const double &deltaT) {
  for (int i = 0; i < 3; ++i) {
    double newV = v[i] + force[i] / mass * deltaT;
    if (newV >= 0) {
      newV = min(newV, 1.5);
    } else {
      newV = max(newV, -1.5);
    }
    pos[i] += (newV + v[i]) / 2 * deltaT;
    v[i] = newV;
  }
};

static double distance(shared_ptr<Object> a, shared_ptr<Object> b) {
  double sum = 0;
  for (int i = 0; i < 3; ++i) {
    sum += (a->pos[i] - b->pos[i]) * (a->pos[i] - b->pos[i]);
  }
  return sqrt(sum) - a->radius - b->radius;
}

static double distance(shared_ptr<Object> a, const vec3 &b) {
  double sum = 0;
  for (int i = 0; i < 3; ++i) {
    sum += (a->pos[i] - b[i]) * (a->pos[i] - b[i]);
  }
  return sqrt(sum);
}

void FrameSystem::calForce() {
  for (int i = 0; i < objects.size(); ++i) {
    if (objects[i]->type != OBJ_GROUP) {
      continue;
    }
    for (int k = 0; k < 3; ++k) {
      objects[i]->force[k] = 0;
    }

    for (int j = 0; j < objects.size(); ++j) {
      if (j == i) {
        continue;
      }
      double dis = distance(objects[i], objects[j]);
      if (objects[j]->type == OBJ_FOOD) {
        for (int k = 0; k < 3; ++k) {
          objects[i]->force[k] += (objects[j]->pos[k] - objects[i]->pos[k]) /
                                  (dis)*objects[i]->forces->food;
        }
      }
      if (objects[j]->type == OBJ_BARRIER && dis < objects[j]->radius) {
        for (int k = 0; k < 3; ++k) {
          objects[i]->force[k] += (objects[i]->pos[k] - objects[j]->pos[k]) /
                                  (dis)*objects[i]->forces->barrier;
        }
      }
      if (objects[j]->type == OBJ_GROUP) {
        if (dis < 5) {
          for (int k = 0; k < 3; ++k) {
            objects[i]->force[k] += (objects[i]->pos[k] - objects[j]->pos[k]) /
                                    (dis * dis * dis) *
                                    objects[i]->forces->repulsion;
          }
        } else if (dis > 10) {
          for (int k = 0; k < 3; ++k) {
            objects[i]->force[k] += (objects[j]->pos[k] - objects[i]->pos[k]) /
                                    (dis * dis * dis) *
                                    objects[i]->forces->group;
          }
        }
      }
    }
    // for (int k = 0; k < 3; ++k) {
    //   LOG(ERROR) << objects[i]->force[k];
    // }
  }
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
    object->calPos(cgSystem->frameSystem->deltaT);
    object->calFrame();
  }
  cgSystem->frameSystem->calForce();
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
