#include "SystemDS.h"
#include "Loader-inl.h"
#include "Perlin-inl.h"

#include <cmath>
#include <glog/logging.h>
#include <iostream>

using namespace std;

namespace ICG {
const double PI = acos(-1);
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
void GLUTSystem::update(void) { cgSystem->frameSystem->frameCounter++; }
// draw model func
void GLUTSystem::drawModel() {
  PerlinNoise pn;
  glPushMatrix();

  const auto fSystem = cgSystem->frameSystem;
  glBegin(GL_POINTS);
  glColor3f(0.0f, 1.0f, 0.0f);
  for (int i = 0; i < fSystem->x; ++i) {
    for (int j = 0; j < fSystem->z; ++j) {
      double unit = 1.0 / fSystem->unit;
      for (int ii = 0; ii < fSystem->unit; ++ii) {
        for (int jj = 0; jj < fSystem->unit; ++jj) {
          double x = i + ii * unit, z = j + jj * unit;
          double y = sin(x + fSystem->frameCounter / 2 / PI) *
                     log(x / fSystem->x + 1) * fSystem->height;
          double tmp = x + fSystem->frameCounter / 2 / PI;
          while (tmp > 2 * PI) {
            tmp -= 2 * PI;
          }
          double noise = pn.noise(tmp / 2 / PI, 0.8, 0.8) * fSystem->height;
          glVertex3f(fSystem->x / 2 - x, y - 20 + noise, -z - fSystem->z / 1.5);
        }
      }
    }
  }
  glEnd();
  glPopMatrix();
}
// callback for dispaly
void GLUTSystem::render(void) {
  // clear buffer
  glClearColor(0, 0, 0, 0);
  glClearDepth(1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render state
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  glLoadIdentity();

  // render wave
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

} // namespace ICG
