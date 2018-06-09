#pragma once


#include <GLUT/glut.h>
#include <vector>
using namespace std;

namespace ICG {
typedef array<double, 3> vec3;

struct Window {
  int height{600};
  int width{800};
  int xPosition{100};
  int yPosition{100};
};

class FrameSystem {
public:
  int frameCounter{0};
  int fps{20};

  int x;
  int z;
  int unit;
  double height;
  int t;
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
  static void drawModel();
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
