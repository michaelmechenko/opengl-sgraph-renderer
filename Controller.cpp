/**
 * @file Controller.cpp
 * @brief Implementation of the Controller class for managing the scenegraph,
 * rendering, and input.
 */

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

/**
 * @brief Controller constructor.
 *
 * Initializes the Controller with references to the model, view,
 * scene file string, and a flag determining text rendering.
 *
 * @param m Reference to the model.
 * @param v Reference to the view.
 * @param f Reference to the filename string for the scene file.
 * @param t Reference to the boolean flag for text-based rendering.
 */
Controller::Controller(Model &m, View &v, string &f, bool &t) {
  model = m;
  view = v;
  file = f;
  isTextRender = t;
  initScenegraph();
}

/**
 * @brief Auxiliary struct to expose texture paths.
 *
 * Inherits from sgraph::ScenegraphImporter to expose the protected
 * method getTexturePaths.
 */
struct SgraphImporterAccessor : public sgraph::ScenegraphImporter {
  using sgraph::ScenegraphImporter::getTexturePaths;
};

/**
 * @brief Initializes the scenegraph.
 *
 * Depending on the file input, loads the scenegraph from a file,
 * sets up the scenegraph in the model, and assigns texture paths.
 */
void Controller::initScenegraph() {
  // Select one of multiple possible scene description files.
  // Uncomment the desired input file below.
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

  // Set texture paths using the exposed accessor for getTexturePaths.
  model.setTexturePaths(
      static_cast<SgraphImporterAccessor &>(importer).getTexturePaths());
}

/**
 * @brief Destructor for Controller.
 *
 * Performs any necessary cleanup.
 */
Controller::~Controller() {}

/**
 * @brief Main execution loop for the Controller.
 *
 * Sets up the animation, initializes the view with the meshes from the
 * scenegraph, and enters the render/update loop. For text-based rendering, the
 * scenegraph is displayed once; for graphical display, a loop updates
 * animations and renders continuously until the window is closed.
 */
void Controller::run() {
  sgraph::IScenegraph *scenegraph = model.getScenegraph();

  // Compute current time and set a global rotation angle based on time.
  double currentTime = glfwGetTime();
  float angle =
      static_cast<float>(currentTime); // Compute a simple time-based angle

  // Create a global animation transformation matrix rotating around the Y-axis.
  glm::mat4 globalAnim =
      glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));

  // Apply the global animation transform to the root node if it is a
  // ParentSGNode.
  if (ParentSGNode *pg = dynamic_cast<ParentSGNode *>(scenegraph->getRoot())) {
    pg->setAnimTransform(globalAnim);
  }

  // Retrieve meshes from the scenegraph and initialize the view.
  map<string, util::PolygonMesh<VertexAttrib>> meshes = scenegraph->getMeshes();
  view.init(this, meshes, this->isTextRender, model.getTexturePaths());
  double lastTime = glfwGetTime();

  if (this->isTextRender) {
    // For text rendering, display the scenegraph once.
    view.display(scenegraph);
  } else {
    // For graphical rendering, continuously update and display the scenegraph.
    while (!view.shouldWindowClose()) {
      double currentTime = glfwGetTime();
      double dt = currentTime - lastTime;
      lastTime = currentTime;

      // Update rotation based on current time.
      float angle = static_cast<float>(currentTime);
      // The following commented code shows an alternative method for updating
      // the animation transform: glm::mat4 globalAnim =
      //     glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
      // if (ParentSGNode *pg =
      //         dynamic_cast<ParentSGNode *>(scenegraph->getRoot()))
      //   pg->setAnimTransform(globalAnim);

      // Traverse the scenegraph to update animated nodes.
      AnimationVisitor animVisitor(dt);
      scenegraph->getRoot()->accept(&animVisitor);
      view.display(scenegraph);
    }
  }
  // Close the rendering window and exit.
  view.closeWindow();
  exit(EXIT_SUCCESS);
}

/**
 * @brief Callback for keyboard input.
 *
 * Handles key press events. Resets the camera with 'R' and sets output flag
 * with 'S'.
 *
 * @param key The key that was pressed.
 * @param scancode The system-specific scancode of the key.
 * @param action Action associated with the key press.
 * @param mods Modifier keys pressed.
 */
void Controller::onkey(int key, int scancode, int action, int mods) {
  if (action != GLFW_PRESS)
    return;
  char keyPress = static_cast<char>(key);
  switch (keyPress) {
  case 'R':
    // Reset the camera when 'R' is pressed.
    view.resetCamera();
    break;
  }
  // If 'S' key is pressed, set the flag to output view details.
  if (action == GLFW_PRESS && key == GLFW_KEY_S) {
    View::shouldOutput = true;
  }
}

/**
 * @brief Callback for mouse button press events.
 *
 * Updates the flag indicating whether the left mouse button is pressed.
 *
 * @param button The mouse button that was pressed.
 * @param action The action (press/release) for the mouse button.
 * @param mods Modifier keys pressed.
 */
void Controller::onmousepress(int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT)
    isMousePressed = (action == GLFW_PRESS);
}

/**
 * @brief Callback for mouse movement events.
 *
 * Adjusts the camera orientation based on mouse movement when dragging.
 *
 * @param xpos The current x-coordinate of the mouse.
 * @param ypos The current y-coordinate of the mouse.
 */
void Controller::onmousemove(double xpos, double ypos) {
  float mouseSens = 6.0f;
  if (isMousePressed) {
    // Calculate the difference in mouse position.
    float xDiff = static_cast<float>(prev_x_pos - xpos) * mouseSens;
    float yDiff = static_cast<float>(ypos - prev_y_pos) * mouseSens;
    yDiff = -yDiff; // Invert y-axis difference for correct orientation.
    // Rotate the camera based on mouse movement.
    view.rotateCamera(glm::radians(yDiff), glm::radians(xDiff));
  }
  // Update previous mouse positions.
  prev_x_pos = xpos;
  prev_y_pos = ypos;
}

/**
 * @brief Callback for window reshape events.
 *
 * Adjusts the viewport dimensions and prints the new size.
 *
 * @param width New width of the window.
 * @param height New height of the window.
 */
void Controller::reshape(int width, int height) {
  cout << "Window reshaped to width=" << width << " and height=" << height
       << endl;
  glViewport(0, 0, width, height);
}

/**
 * @brief Cleans up resources.
 *
 * Closes the view window ensuring proper cleanup when disposing the Controller.
 */
void Controller::dispose() { view.closeWindow(); }

/**
 * @brief Error callback function.
 *
 * Logs any GLFW errors encountered.
 *
 * @param error Error code.
 * @param description Description of the error.
 */
void Controller::error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}
