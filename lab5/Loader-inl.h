#pragma once

#include "SystemDS.h"

#include <GLUT/glut.h>
#include <array>
#include <cassert>
#include <fstream>
#include <glog/logging.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
// constant define

namespace ICG {

namespace color {
static unordered_map<string, array<GLdouble, 3>> colorMap{
    {"green", array<GLdouble, 3>{0.0f, 1.0f, 0.0f}},
    {"orange", array<GLdouble, 3>{1.0f, 0.5f, 0.0f}},
    {"red", array<GLdouble, 3>{1.0f, 0.0f, 0.0f}},
    {"yellow", array<GLdouble, 3>{1.0f, 1.0f, 0.0f}},
    {"blue", array<GLdouble, 3>{0.0f, 0.0f, 1.0f}},
    {"magenta", array<GLdouble, 3>{1.0f, 0.0f, 1.0f}},
    {"purple", array<GLdouble, 3>{1.0f, 0.0f, 1.0f}},
    {"gold", array<GLdouble, 3>{1.0f, 0.0f, 0.84f}},
    {"white", array<GLdouble, 3>{1.0f, 1.0f, 1.0f}},
    {"black", array<GLdouble, 3>{0.0f, 0.0f, 0.0f}},
};
} // namespace color

struct Face {
  vector<int> vertex;
  string material;
};

class Loader {
public:
  static bool loadDesFile(const string &fileName,
                          const shared_ptr<FrameSystem> fSystem) {
    ifstream desFile(fileName, ios::in | ios::binary);
    // check if opened correctl
    if (not desFile.is_open()) {
      LOG(ERROR) << "Cannot open desFrame file: " << fileName;
      return false;
    }
    string line;
    while (!desFile.eof()) {
      getline(desFile, line);
      if (line.size() == 0) {
        continue;
      }

      istringstream lineStream(line);
      string token;
      lineStream >> token;
      if (token == "#") {
        continue;
      } else if (token == "wave") {
        lineStream >> fSystem->x >> fSystem->z >> fSystem->unit >>
            fSystem->height >> fSystem->t;
      }
    }
    return true;
  }
};
} // namespace ICG
