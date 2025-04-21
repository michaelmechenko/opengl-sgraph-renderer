#include "Controller.h"
#include "ObjImporter.h"
#include "sgraph/AnimationVisitor.h"
#include "sgraph/ParentSGNode.h"
#include "sgraph/ScenegraphImporter.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>

using namespace sgraph;
using namespace std;

Controller::Controller(Model &m, View &v, string &f, bool &t) {
  model = m;
  view = v;
  file = f;
  isTextRender = t;
  initScenegraph();
}

struct SgraphImporterAccessor : public sgraph::ScenegraphImporter {
  using sgraph::ScenegraphImporter::getTexturePaths;
};

void Controller::initScenegraph() {
  // string inFileStr = (file == "") ? "scenegraphmodels/input.txt" : file;
  // string inFileStr = (file == "") ? "scenegraphmodels/funscene.txt" : file;
  string inFileStr =
      (file == "") ? "scenegraphmodels/spheres_on_opposite_sides_of_wall.txt"
                   : file;
  // string inFileStr = (file == "") ? "scenegraphmodels/box.txt" : file;
  // string inFileStr = (file == "") ? "scenegraphmodels/spheres.txt" : file;
  ifstream inFile(inFileStr);
  ScenegraphImporter importer;
  IScenegraph *scenegraph = importer.parse(inFile);
  model.setScenegraph(scenegraph);

  model.setTexturePaths(
      static_cast<SgraphImporterAccessor &>(importer).getTexturePaths());
}

Controller::~Controller() {}

void Controller::run() {
  sgraph::IScenegraph *scenegraph = model.getScenegraph();
  // update the root animation transform (applied to entire scene)
  double currentTime = glfwGetTime();
  float angle = static_cast<float>(currentTime); // predefined function of time

  glm::mat4 globalAnim =
      glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));

  if (ParentSGNode *pg = dynamic_cast<ParentSGNode *>(scenegraph->getRoot())) {
    pg->setAnimTransform(globalAnim);
  }

  // initialize view with meshes from the scenegraph
  map<string, util::PolygonMesh<VertexAttrib>> meshes = scenegraph->getMeshes();
  view.init(this, meshes, this->isTextRender, model.getTexturePaths());
  double lastTime = glfwGetTime();
  if (this->isTextRender) {
    view.display(scenegraph);
  } else {
    while (!view.shouldWindowClose()) {
      double currentTime = glfwGetTime();
      double dt = currentTime - lastTime;
      lastTime = currentTime;
      // update global animation transform for the root node
      float angle = static_cast<float>(currentTime);
      // glm::mat4 globalAnim =
      //     glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
      // if (ParentSGNode *pg =
      //         dynamic_cast<ParentSGNode *>(scenegraph->getRoot()))
      //   pg->setAnimTransform(globalAnim);
      // traverse the scenegraph to update animated nodes
      AnimationVisitor animVisitor(dt);
      scenegraph->getRoot()->accept(&animVisitor);
      view.display(scenegraph);
    }
  }
  view.closeWindow();
  exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods) {
  if (action != GLFW_PRESS)
    return;
  char keyPress = static_cast<char>(key);
  switch (keyPress) {
  case 'R':
    view.resetCamera();
    break;
  }
  if (action == GLFW_PRESS && key == GLFW_KEY_S) {
    View::shouldOutput = true;
  }
}

void Controller::onmousepress(int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT)
    isMousePressed = (action == GLFW_PRESS);
}

void Controller::onmousemove(double xpos, double ypos) {
  float mouseSens = 6.0f;
  if (isMousePressed) {
    float xDiff = static_cast<float>(prev_x_pos - xpos) * mouseSens;
    float yDiff = static_cast<float>(ypos - prev_y_pos) * mouseSens;
    yDiff = -yDiff;
    view.rotateCamera(glm::radians(yDiff), glm::radians(xDiff));
  }
  prev_x_pos = xpos;
  prev_y_pos = ypos;
}

void Controller::reshape(int width, int height) {
  cout << "Window reshaped to width=" << width << " and height=" << height
       << endl;
  glViewport(0, 0, width, height);
}

void Controller::dispose() { view.closeWindow(); }
void Controller::error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}
