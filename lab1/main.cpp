// custom lib
#include "SystemDS.h"
// standard
#include <cassert>
#include <cmath>
#include <iostream>

#include <gflags/gflags.h>
#include <glog/logging.h>
// glut
#include <GLUT/glut.h>

DEFINE_string(obj_file, "../files/porsche.obj", "path to the obj File");
DEFINE_string(control_file, "../files/EULAR_CatmullRom.in",
              "path to the control File");

using namespace ICG;

int main(int argc, char *argv[]) {
  // init glog and glags
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // init CoreCGSystem
  auto cgSystem = make_shared<CoreCGSystem>();

  // create opengL window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(cgSystem->window->width, cgSystem->window->height);
  glutInitWindowPosition(cgSystem->window->xPosition,
                         cgSystem->window->yPosition);
  glutCreateWindow(argv[0]);

  // load Files
  cgSystem->loadDataFromFile(FLAGS_obj_file, FLAGS_control_file);
  // init GLUTSystem
  GLUTSystem::init(cgSystem);

  // set callback functions
  glutDisplayFunc(GLUTSystem::render);
  glutReshapeFunc(GLUTSystem::reshape);
  glutKeyboardFunc(GLUTSystem::keyboard);
  glutTimerFunc(1000.0 / cgSystem->frameSystem->fps, GLUTSystem::timer, 0);
  // main loop
  glutMainLoop();

  return 0;
}
