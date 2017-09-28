#pragma once

#include <GLUT/glut.h>
#include <array>
#include <iostream>

using namespace std;

namespace ICG {
struct Quaternion {
  GLdouble w, x, y, z;
};

ostream &operator<<(ostream &os, const Quaternion &quat) {
  os << "(" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z
     << ")";
  return os;
}

class TransMatrix {
public:
  std::array<GLdouble, 16> mat;
  TransMatrix(Quaternion quat)
      : mat({1 - 2 * quat.y * quat.y - 2 * quat.z * quat.z,
             2 * (quat.x * quat.y + quat.w * quat.z),
             2 * (quat.x * quat.z - quat.w * quat.y), 0,
             2 * (quat.x * quat.y - quat.w * quat.z),
             1 - 2 * quat.x * quat.x - 2 * quat.z * quat.z,
             2 * (quat.y * quat.z + quat.w * quat.x), 0,
             2 * (quat.x * quat.z + quat.w * quat.y),
             2 * (quat.y * quat.z - quat.w * quat.x),
             1 - 2 * quat.x * quat.x - 2 * quat.y * quat.y, 0, 0, 0, 0, 1}){};
};
} // namespace ICG
