#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "Callbacks.h"
#include "Model.h"
#include "View.h"

/**
 * @brief Controller class manages the interaction between the model and view.
 *
 * This class handles various callbacks and manages the running of the main
 * loop, interaction events, and scene graph initialization.
 */
class Controller : public Callbacks {
public:
  /**
   * @brief Constructs a new Controller object.
   *
   * Initializes the Controller with references to the model and view, along
   * with additional configuration parameters.
   *
   * @param m Reference to the Model instance.
   * @param v Reference to the View instance.
   * @param s Reference to a string representing the file name.
   * @param t Reference to a boolean flag indicating text rendering mode.
   */
  Controller(Model &m, View &v, string &s, bool &t);

  /**
   * @brief Destroys the Controller object.
   */
  ~Controller();

  /**
   * @brief Runs the main loop of the application.
   *
   * This method starts the event loop where the application continuously polls
   * for events and updates the view.
   */
  void run();

  /**
   * @brief Reshapes the viewport.
   *
   * Called when the window is resized.
   *
   * @param width New width of the window.
   * @param height New height of the window.
   */
  virtual void reshape(int width, int height);

  /**
   * @brief Disposes of resources.
   *
   * Called during cleanup to free resources before the application closes.
   */
  virtual void dispose();

  /**
   * @brief Handles key events.
   *
   * Called when a key event occurs.
   *
   * @param key The key code.
   * @param scancode The scancode of the key.
   * @param action The action (e.g., press, release).
   * @param mods Modifier keys pressed (e.g., Shift, Ctrl).
   */
  virtual void onkey(int key, int scancode, int action, int mods);

  /**
   * @brief Handles mouse button events.
   *
   * Called when a mouse button is pressed or released.
   *
   * @param button The mouse button code.
   * @param action The action (press or release).
   * @param mods Modifier keys pressed during the event.
   */
  virtual void onmousepress(int button, int action, int mods);

  /**
   * @brief Handles mouse movement events.
   *
   * Called when the mouse is moved.
   *
   * @param xpos The new x-position of the mouse.
   * @param ypos The new y-position of the mouse.
   */
  virtual void onmousemove(double xpos, double ypos);

  /**
   * @brief Handles error callbacks.
   *
   * Called when an error occurs.
   *
   * @param error Error code.
   * @param description Description of the error.
   */
  virtual void error_callback(int error, const char *description);

private:
  /**
   * @brief Initializes the scene graph.
   *
   * Sets up the scene graph elements required for rendering.
   */
  void initScenegraph();

  View view;             ///< View component for rendering.
  Model model;           ///< Model component for data management.
  string file;           ///< File name associated with the controller.
  bool isTextRender;     ///< Flag indicating if text rendering is active.
  double prev_x_pos = 0; ///< Previous x-coordinate of the mouse.
  double prev_y_pos = 0; ///< Previous y-coordinate of the mouse.
  bool isMousePressed =
      false; ///< Flag that indicates if the mouse is currently pressed.
};

#endif
