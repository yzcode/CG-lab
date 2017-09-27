#pragma once

#include <GLUT/glut.h>
#include <array>
#include <cassert>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
// constant define

namespace ICG {

namespace color {
  static constexpr array<GLfloat, 3> green{0.0f, 1.0f, 0.0f};
  static constexpr array<GLfloat, 3> orange{1.0f, 0.5f, 0.0f};
  static constexpr array<GLfloat, 3> red{1.0f, 0.0f, 0.0f};
  static constexpr array<GLfloat, 3> yellow{1.0f, 1.0f, 0.0f};
  static constexpr array<GLfloat, 3> blue{0.0f, 0.0f, 1.0f};
  static constexpr array<GLfloat, 3> magenta{1.0f, 0.0f, 1.0f};
  static constexpr array<GLfloat, 3> purple{1.0f, 0.0f, 1.0f};
  static constexpr array<GLfloat, 3> gold{1.0f, 0.0f, 0.84f};
}

class ObjLoader {
private:

public:
  static GLuint loadObjFromFile(const string &fileName) {
    auto newObjID = glGenLists(1);
    ifstream objFile(fileName, ios::in | ios::binary);
    // check if opened correctl
    if (not objFile.is_open()) {
      LOG(FATAL) << "Cannot open obj file: " << fileName;
    }
    // input vectors
    vector<array<GLfloat, 3>> points;
    // parser
    string line;
    while (!objFile.eof()) {
      getline(objFile, line);
      istringstream lineStream(line);
      string token;
      lineStream >> token;
      if (token == "#") {
        continue;
      } else if (token == "v") {
        array<GLfloat, 3> point;
        lineStream >> point[0] >> point[1] >> point[2];
        points.emplace_back(point);
      }
    }
    // insert points to GL
    glPointSize(2.0);
    glNewList(newObjID, GL_COMPILE);
    {
      glPushMatrix();
      glBegin(GL_POINTS);
      for (const auto &point : points) {
        glVertex3f(point[0], point[1], point[2]);
      }
      glEnd();
    }
    glPopMatrix();
    glEndList();

    // close file
    objFile.close();
    return newObjID;
  }
};
} // namespace ICG
