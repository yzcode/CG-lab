#pragma once

#include "MatrixOp-inl.h"
#include <glog/logging.h>
using namespace std;

namespace ICG {
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
}
