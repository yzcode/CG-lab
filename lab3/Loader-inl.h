#pragma once

#include "Frame-inl.h"
#include "MatrixOp-inl.h"
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
  static bool loadControlInfoFromFile(const string &fileName,
                                      shared_ptr<Object> object) {
    object->keyFrames = make_shared<vector<Frame>>();

    ifstream frameFile(fileName, ios::in | ios::binary);
    // check if opened correctl
    if (not frameFile.is_open()) {
      LOG(ERROR) << "Cannot open KeyFrame file: " << fileName;
      return false;
    }

    string line;
    while (!frameFile.eof()) {
      getline(frameFile, line);
      if (line.size() == 0) {
        continue;
      }

      istringstream lineStream(line);
      string token;
      lineStream >> token;
      if (token == "#") {
        continue;
      } else if (token == "interpolater") {
        lineStream >> token;
        if (token == "BSPLINE") {
          object->interpolater = make_shared<BSplineInterpolation>();
        } else if (token == "CATMULLROM") {
          object->interpolater = make_shared<CatmullRomInterpolation>();
        } else if (token == "Linear") {
          object->interpolater = make_shared<LineInterpolation>();
        } else {
          LOG(ERROR) << "Unknow interpolater: " << token;
        }
      } else if (token == "kf") {
        vector<GLdouble> lineVec;
        GLdouble vertexIndex;
        while (lineStream >> vertexIndex) {
          lineVec.emplace_back(vertexIndex);
        }
        // insert frame
        object->keyFrames->emplace_back(Frame{lineVec, false});
      }
    }

    return true;
  }

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
      } else if (token == "dt") {
        lineStream >> fSystem->deltaT;
      } else if (token == "object") {
        string objFile, controlFile;
        int fatherID;
        shared_ptr<Object> newObj = make_shared<Object>();

        lineStream >> objFile >> controlFile >> fatherID >> newObj->phase;

        newObj->modelID = loadObjFromFile(objFile);
        loadControlInfoFromFile(controlFile, newObj);
        fSystem->objects.emplace_back(newObj);
        if (fatherID != -1) {
          lineStream >> newObj->joint[0] >> newObj->joint[1] >>
              newObj->joint[2];
          newObj->parent = fSystem->objects[fatherID];
          fSystem->objects[fatherID]->sons.emplace_back(newObj);
        }
      }
    }
    return true;
  }

  static GLuint loadObjFromFile(const string &fileName) {
    auto newObjID = glGenLists(1);
    ifstream objFile(fileName, ios::in | ios::binary);
    // check if opened correctl
    if (not objFile.is_open()) {
      LOG(FATAL) << "Cannot open obj file: " << fileName;
    }
    // input vectors
    vector<array<GLfloat, 3>> points;
    vector<Face> faces;
    // parser
    string line;
    string curMaterial = "";

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
      } else if (token == "f") {
        Face newFace{{}, curMaterial};
        while (lineStream >> token) {
          istringstream tokenStream(token);
          int index;
          tokenStream >> index;
          newFace.vertex.emplace_back(index - 1);
        }
        faces.emplace_back(newFace);
      } else if (token == "usemtl") {
        lineStream >> curMaterial;
      }
    }
    // insert points to GL
    glPointSize(2.0);
    glNewList(newObjID, GL_COMPILE);
    {
      glPushMatrix();
      for (const auto &face : faces) {
        if (color::colorMap.count(face.material)) {
          glBegin(GL_POLYGON);
          const auto &materialColor = color::colorMap[face.material];
          glColor3d(materialColor[0], materialColor[1], materialColor[2]);
        } else {
          glBegin(GL_LINE_LOOP);
        }

        for (const auto &vertexIndex : face.vertex) {
          const auto &point = points[vertexIndex];
          glVertex3f(point[0], point[1], point[2]);
        }
        glEnd();
      }
      glPopMatrix();
    }
    glEndList();
    // close file
    objFile.close();
    return newObjID;
  }
};
} // namespace ICG
