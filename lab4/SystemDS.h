#pragma once

#include "Frame-inl.h"

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

enum ObjType {
    OBJ_BARRIER = 0,
    OBJ_FOOD,
    OBJ_GROUP
};

struct Forces {
  double food;
  double barrier;
  double group;
  double repulsion;
};

class Object {
public:
  const static double g;

  GLuint modelID{0};
  shared_ptr<Frame> curFrame;
  ObjType type;
  shared_ptr<Forces> forces;

  double radius;
  double mass{1};
  vec3 v;
  vec3 pos;
  vec3 force;

  void calFrame();
  void calPos(const double& deltaT);
};

class FrameSystem {
public:
  int frameCounter{0};
  double deltaT{0.01};
  double offsetT{0};
  int fps{120};

  vector<shared_ptr<Object>> objects;

  void calForce();
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
  static void drawModel(shared_ptr<Object> object, bool trans = true);
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
