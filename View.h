#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include "Callbacks.h"
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/RaycastScenegraphRenderer.h"
#include "sgraph/SGNodeVisitor.h"
#include <GLFW/glfw3.h>
#include <ShaderProgram.h>
#include <glad/glad.h>
#include <map>
#include <stack>
#include <string>

using namespace std;

/**
 * @brief The View class encapsulates the rendering view and the associated
 * OpenGL window.
 *
 * This class handles initialization, scenegraph rendering, camera manipulation,
 * and window management.
 */
class View {
public:
  /**
   * @brief Constructs a new View object.
   */
  View();

  /**
   * @brief Destroys the View object and cleans up allocated resources.
   */
  ~View();

  /**
   * @brief Initializes the view with the provided callbacks, meshes, rendering
   * mode, and texture paths.
   *
   * @param callbacks Pointer to the application's callback functions.
   * @param meshes A map associating mesh names with their corresponding polygon
   * mesh objects.
   * @param isTextRender Boolean flag indicating if text rendering mode is
   * enabled.
   * @param texturePaths A map linking texture names to their file paths.
   */
  void init(Callbacks *callbacks,
            map<string, util::PolygonMesh<VertexAttrib>> &meshes,
            bool isTextRender, map<string, string> texturePaths);

  /**
   * @brief Renders the provided scenegraph.
   *
   * @param scenegraph Pointer to the scenegraph that needs to be rendered.
   */
  void display(sgraph::IScenegraph *scenegraph);

  /**
   * @brief Rotates the camera by the specified pitch and yaw angles.
   *
   * @param pitch The pitch angle to rotate the camera.
   * @param yaw The yaw angle to rotate the camera.
   */
  void rotateCamera(float pitch, float yaw);

  /**
   * @brief Resets the camera to its default orientation and position.
   */
  void resetCamera();

  /**
   * @brief Checks if the window should be closed.
   *
   * @return true if the window should close, otherwise false.
   */
  bool shouldWindowClose();

  /**
   * @brief Closes the window and performs necessary cleanup.
   */
  void closeWindow();

  /**
   * @brief Static flag to control output generation.
   */
  static bool shouldOutput;

private:
  /**
   * @brief Pointer to the GLFW window used for rendering.
   */
  GLFWwindow *window;

  /**
   * @brief Shader program used for rendering operations.
   */
  util::ShaderProgram program;

  /**
   * @brief Vault that stores shader locations.
   */
  util::ShaderLocationsVault shaderLocations;

  /**
   * @brief Map of object instances keyed by their names.
   */
  map<string, util::ObjectInstance *> objects;

  /**
   * @brief Projection matrix used for transforming 3D coordinates to 2D.
   */
  glm::mat4 projection;

  /**
   * @brief Stack of modelview matrices for managing hierarchical
   * transformations.
   */
  stack<glm::mat4> modelview;

  /**
   * @brief Renderer for performing raycasting-based scenegraph rendering.
   */
  sgraph::RaycastScenegraphRenderer *rayRenderer;

  /**
   * @brief Renderer for handling OpenGL-based scenegraph rendering.
   */
  sgraph::GLScenegraphRenderer *renderer;

  /**
   * @brief Map storing OpenGL texture identifiers by texture name.
   */
  map<string, GLuint> textures;

  /**
   * @brief Default texture identifier used when a specific texture is not
   * available.
   */
  GLuint defaultTexture;

  /**
   * @brief Boolean flag indicating whether text rendering mode is active.
   */
  bool isTextRender;
};

#endif
