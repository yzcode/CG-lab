#pragma once

#include "Frame-inl.h"
#include <cmath>

using namespace std;

namespace ICG {
class BaseInterpolation {
public:
  virtual shared_ptr<Frame> interpolation(shared_ptr<vector<Frame>> keyframes,
                                          const int &curKeyFrame,
                                          double deltaT) {
    return make_shared<Frame>(keyframes->at(curKeyFrame));
  };
};

class TwoPointsInterpolation : public BaseInterpolation {
public:
  virtual shared_ptr<Frame> interpolation(shared_ptr<vector<Frame>> keyframes,
                                          const int &curKeyFrame,
                                          double deltaT) {
    int curKeyFrameInx = curKeyFrame % (keyframes->size() - 1);
    if (curKeyFrameInx < 0 || curKeyFrameInx + 1 >= keyframes->size()) {
      LOG(FATAL) << "Expect curKeyFrameInx index between 2 to "
                 << keyframes->size() - 2 << ": " << curKeyFrameInx;
    }
    auto aVec = keyframes->at(curKeyFrameInx).getData();
    auto bVec = keyframes->at(curKeyFrameInx + 1).getData();
    vector<GLdouble> nVec;
    // interpolation
    for (int i = 0; i < aVec.size(); ++i) {
      nVec.emplace_back(interValue(aVec[i], bVec[i], deltaT));
    }
    auto interFrame = make_shared<Frame>(nVec);
    return interFrame;
  }

  virtual GLdouble interValue(GLdouble p0, GLdouble p1, double deltaT) {
    return p1;
  }
};

class LineInterpolation : public TwoPointsInterpolation {
public:
  GLdouble interValue(GLdouble a, GLdouble b, double deltaT) {
    return (1.0 - deltaT) * a + b * deltaT;
  }
};

class FourPointsInterpolation : public BaseInterpolation {
public:
  virtual shared_ptr<Frame> interpolation(shared_ptr<vector<Frame>> keyframes,
                                          const int &curKeyFrame,
                                          double deltaT) {
    int curKeyFrameInx = curKeyFrame % (keyframes->size() - 3) + 1;
    if (curKeyFrameInx - 1 < 0 || curKeyFrameInx + 2 >= keyframes->size()) {
      LOG(FATAL) << "Expect curKeyFrameInx index between 2 to "
                 << keyframes->size() - 3 << ": " << curKeyFrameInx;
    }
    auto a0Vec = keyframes->at(curKeyFrameInx - 1).getData();
    auto aVec = keyframes->at(curKeyFrameInx).getData();
    auto bVec = keyframes->at(curKeyFrameInx + 1).getData();
    auto b0Vec = keyframes->at(curKeyFrameInx + 2).getData();
    vector<GLdouble> nVec;
    // interpolation
    for (int i = 0; i < aVec.size(); ++i) {
      nVec.emplace_back(
          interValue(a0Vec[i], aVec[i], bVec[i], b0Vec[i], deltaT));
    }
    auto interFrame = make_shared<Frame>(nVec);
    return interFrame;
  }

  virtual GLdouble interValue(GLdouble p0, GLdouble p1, GLdouble p2,
                              GLdouble p3, double deltaT) {
    return p1;
  }
};

class CatmullRomInterpolation : public FourPointsInterpolation {
public:
  virtual GLdouble interValue(GLdouble p0, GLdouble p1, GLdouble p2,
                              GLdouble p3, double deltaT) {
    return 0.5 * ((2 * p1) + (-p0 + p2) * deltaT +
                  (2 * p0 - 5 * p1 + 4 * p2 - p3) * deltaT * deltaT +
                  (-p0 + 3 * p1 - 3 * p2 + p3) * deltaT * deltaT * deltaT);
  }
};

class BSplineInterpolation : public FourPointsInterpolation {
public:
  virtual GLdouble interValue(GLdouble p0, GLdouble p1, GLdouble p2,
                              GLdouble p3, double deltaT) {
    return (p0 + 4 * p1 + p2) / 6 - (p0 - p2) / 2 * deltaT +
           (p0 - 2 * p1 + p2) / 2 * deltaT * deltaT -
           (p0 - 3 * p1 + 3 * p2 - p3) / 6 * deltaT * deltaT * deltaT;
  }
};

} // namespace ICG
