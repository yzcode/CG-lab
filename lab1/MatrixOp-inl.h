#pragma once

#include <GLUT/glut.h>
#include <array>
#include <cmath>
#include <iostream>

using namespace std;

namespace ICG {

const double PI = acos(-1.0);
struct Quaternion {
  GLdouble w, x, y, z;
};

struct EulerAngles {
  GLdouble x, y, z;
  EulerAngles(GLdouble xx, GLdouble yy, GLdouble zz, bool radian = true) {
    x = xx;
    y = yy;
    z = zz;
    if (!radian) {
      x = xx / 360 * 2 * PI;
      y = yy / 360 * 2 * PI;
      z = zz / 360 * 2 * PI;
    }
  }
};

ostream &operator<<(ostream &os, const Quaternion &quat) {
  os << "(" << quat.w << ", " << quat.x << ", " << quat.y << ", " << quat.z
     << ")";
  return os;
}

ostream &operator<<(ostream &os, const EulerAngles &quat) {
  os << "(" << quat.x << ", " << quat.y << ", " << quat.z << ")";
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

  TransMatrix(EulerAngles eulera) {
    double sinX = sin(eulera.x), cosX = cos(eulera.x);
    double sinY = sin(eulera.y), cosY = cos(eulera.y);
    double sinZ = sin(eulera.z), cosZ = cos(eulera.z);
    mat[0] = cosZ * cosY;
    mat[1] = sinZ * cosY;
    mat[2] = -sinY;
    mat[4] = cosZ * sinY * sinX - sinZ * cosX;
    mat[5] = sinZ * sinY * sinX + cosZ * cosX;
    mat[6] = cosY * sinX;
    mat[8] = cosZ * sinY * cosX + sinZ * sinX;
    mat[9] = sinZ * sinY * cosX - cosZ * sinX;
    mat[10] = cosY * cosX;
    mat[3] = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
    mat[15] = 1;
  };
};

ostream &operator<<(ostream &os, const TransMatrix &tMatrix) {
  for (int i = 0; i < 4; ++i) {
    os << "| ";
    for (int j = 0; j < 4; ++j) {
      os << tMatrix.mat[j * 4 + i] << " ";
    }
    os << "|" << endl;
  }
  return os;
}

} // namespace ICG
