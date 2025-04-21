#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "Callbacks.h"
#include "Model.h"
#include "View.h"

class Controller : public Callbacks {
public:
  Controller(Model &m, View &v, string &s, bool &t);
  ~Controller();
  void run();

  virtual void reshape(int width, int height);
  virtual void dispose();
  virtual void onkey(int key, int scancode, int action, int mods);
  virtual void onmousepress(int button, int action, int mods);
  virtual void onmousemove(double xpos, double ypos);
  virtual void error_callback(int error, const char *description);

private:
  void initScenegraph();

  View view;
  Model model;
  string file;
  bool isTextRender;
  double prev_x_pos = 0;
  double prev_y_pos = 0;
  bool isMousePressed = false;
};

#endif
