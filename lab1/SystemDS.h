#pragma once

#include "interpolation-inl.h"
#include "Frame-inl.h"

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

struct FrameSystem {
  int frameCounter{0};
  double deltaT{0.01};
  double offsetT{0};
  int curKeyFrame{1};
  shared_ptr<vector<Frame>> keyFrames;
  shared_ptr<Frame> curFrame;
  shared_ptr<BaseInterpolation> interpolater;
  int fps{60};
};

class CoreCGSystem {
public:
  shared_ptr<Window> window;
  shared_ptr<FrameSystem> frameSystem;

  GLuint modelID{0};

  CoreCGSystem(){
    window = make_shared<Window>();
    frameSystem = make_shared<FrameSystem>();
  };

  void loadDataFromFile(const string& objFile, const string& controlFile);
};

class GLUTSystem {
private:
  static shared_ptr<CoreCGSystem> cgSystem;
public:
  static void init(shared_ptr<CoreCGSystem> cgSystem);
  // frameSystem update func
  static void update(void);
  // draw model func
  static void drawModel(void);
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
