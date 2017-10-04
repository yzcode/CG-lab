// custom lib
#include "SystemDS.h"
// standard
#include <cassert>
#include <cmath>
#include <iostream>

#include <glog/logging.h>
// glut
#include <GLUT/glut.h>

using namespace ICG;

int main(int argc, char **argv) {
  // init glog
  google::InitGoogleLogging(argv[0]);

  // init CoreCGSystem
  auto cgSystem = make_shared<CoreCGSystem>();

  // create opengL window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(cgSystem->window.width, cgSystem->window.height);
  glutInitWindowPosition(cgSystem->window.xPosition,
                         cgSystem->window.yPosition);
  glutCreateWindow(argv[0]);

  // load Files
  cgSystem->loadDataFromFile();
  // init GLUTSystem
  GLUTSystem::init(cgSystem);

  // set callback functions
  glutDisplayFunc(GLUTSystem::render);
  glutReshapeFunc(GLUTSystem::reshape);
  glutKeyboardFunc(GLUTSystem::keyboard);
  glutTimerFunc(1000.0 / cgSystem->fps, GLUTSystem::timer, 0);
  // main loop
  glutMainLoop();

  return 0;
}
