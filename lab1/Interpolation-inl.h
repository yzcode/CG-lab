#pragma once

#include "Loader-inl.h"
#include <cmath>

using namespace std;

namespace ICG {
class BaseInterpolation {
public:
  virtual shared_ptr<Frame> interpolation(const Frame &a, const Frame &b,
                                          double deltaT) {
    return make_shared<Frame>(a);
  };
};

class LineInterpolation : public BaseInterpolation {
public:
  virtual shared_ptr<Frame> interpolation(const Frame &a, const Frame &b,
                                          double deltaT) {
    auto aVec = a.getData();
    auto bVec = b.getData();
    vector<GLdouble> nVec;
    // interpolation
    for (int i = 0; i < aVec.size(); ++i) {
      nVec.emplace_back(interValue(aVec[i], bVec[i], deltaT));
    }
    auto interFrame = make_shared<Frame>(nVec);

    return interFrame;
  }

private:
  GLdouble interValue(GLdouble a, GLdouble b, double deltaT) {
    return (1.0 - deltaT) * a + b * deltaT;
  }
};

} // namespace ICG
