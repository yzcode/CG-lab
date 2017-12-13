#pragma once

#include "Frame-inl.h"
#include "interpolation-inl.h"

#include <GLUT/glut.h>
#include <vector>
using namespace std;

namespace ICG {
struct Window {
  int height{600};
  int width{800};
  int xPosition{100};
  int yPosition{100};
};

struct Object {
  GLuint modelID{0};
  shared_ptr<Object> parent{nullptr};
  vector<shared_ptr<Object>> sons;
  array<double, 3> joint{0, 0, 0};
  int phase {0};

  shared_ptr<vector<Frame>> keyFrames;
  shared_ptr<Frame> curFrame;
  shared_ptr<BaseInterpolation> interpolater;
};

struct FrameSystem {
  int frameCounter{0};
  double deltaT{0.01};
  double offsetT{0};
  int curKeyFrame{0};
  int fps{60};
  vector<shared_ptr<Object>> objects;
};

class CoreCGSystem {
public:
  shared_ptr<Window> window;
  shared_ptr<FrameSystem> frameSystem;

  CoreCGSystem() {
    window = make_shared<Window>();
    frameSystem = make_shared<FrameSystem>();
  };

  void loadDataFromFile(const string &desFile);
};

class GLUTSystem {
private:
  static shared_ptr<CoreCGSystem> cgSystem;

public:
  static void init(shared_ptr<CoreCGSystem> cgSystem);
  // frameSystem update func
  static void update(void);
  // draw model func
  static void drawModel(shared_ptr<Object> object);
  // callback for dispaly
  static void render(void);
  // callback for keyboard
  static void keyboard(unsigned char key, int x, int y);
  // callback for reshape
  static void reshape(int w, int h);
  // callback for timer
  static void timer(int value);
};

} // end of namespace ICG
