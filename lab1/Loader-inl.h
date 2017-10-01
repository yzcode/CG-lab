#pragma once

#include "MatrixOp-inl.h"
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

class Frame {
public:
  OrientationType orientationType;
  shared_ptr<Orientation> orientation;
  shared_ptr<TranslationVec> translationVec;
  shared_ptr<ScalingVec> scalingVec;

  Frame(OrientationType orientationType, shared_ptr<Orientation> ori,
        shared_ptr<TranslationVec> tVec,
        shared_ptr<ScalingVec> sVec = make_shared<ScalingVec>())
      : orientationType(orientationType), orientation(ori),
        translationVec(tVec), scalingVec(sVec){};

  Frame(const vector<GLdouble> &vec, bool radian = true) {
    if (vec.size() == 7) { // input for Quaternion
      orientationType = ICG_QUATERNION;
      orientation = make_shared<Quaternion>(vec[3], vec[4], vec[5], vec[6]);
    } else if (vec.size() == 6) { // input for EulerAngles
      orientationType = ICG_EULER;
      orientation = make_shared<EulerAngles>(vec[3], vec[4], vec[5], radian);
    } else {
      LOG(ERROR) << "Unable to parse KeyFrame vec!";
    }
    translationVec = make_shared<TranslationVec>(vec[0], vec[1], vec[2]);

    scalingVec = make_shared<ScalingVec>();
  }
  Frame() {}

  vector<GLdouble> getData() const {
    vector<GLdouble> ret(translationVec->getData());
    auto rVec = orientation->getData();
    ret.insert(ret.end(), rVec.begin(), rVec.end());
    return ret;
  }
};

class Loader {
public:
  static shared_ptr<vector<Frame>>
  loadKeyFramesFromFile(const string &fileName) {
    auto kFrames = make_shared<vector<Frame>>();

    ifstream frameFile(fileName, ios::in | ios::binary);
    // check if opened correctl
    if (not frameFile.is_open()) {
      LOG(FATAL) << "Cannot open KeyFrame file: " << fileName;
    }

    string line;
    while (!frameFile.eof()) {
      getline(frameFile, line);
      if (line.size() == 0) {
        continue;
      }

      istringstream lineStream(line);
      vector<GLdouble> lineVec;
      GLdouble token;
      while (lineStream >> token) {
        lineVec.emplace_back(token);
      }
      // insert frame
      kFrames->emplace_back(Frame{lineVec, false});
    }

    return kFrames;
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
        Face newFace {{}, curMaterial};
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
          const auto& materialColor = color::colorMap[face.material];
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
