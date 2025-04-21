/**
 * @file View.cpp
 * @brief Implementation of the View class and its associated functions for
 * scene graph rendering.
 */

#include "View.h"
#include "GLFW/glfw3.h"
#include "PPMImageLoader.h"
#include "VertexAttrib.h"
#include "sgraph/AbstractSGNode.h"
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/TextScenegraphRenderer.h"
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

using namespace std;

namespace {
// Default camera angles
float cameraPitch = 20.0f; ///< Initial pitch angle for the camera in degrees.
float cameraYaw = -135.0f; ///< Initial yaw angle for the camera in degrees.
} // namespace

bool View::shouldOutput =
    false; ///< Flag to determine if output should be generated.

/**
 * @brief Constructor for the View class.
 *
 * Initializes the default texture to 0.
 */
View::View() : defaultTexture(0) {}

/**
 * @brief Destructor for the View class.
 *
 * Ensures deletion of the default texture if it was created.
 */
View::~View() {
  if (defaultTexture != 0) {
    glDeleteTextures(1, &defaultTexture);
  }
}

/**
 * @brief Recursively collects lights from the scene graph.
 *
 * This function visits all nodes in the scene graph, converts light properties
 * to view coordinates, and accumulates them into the provided lights vector.
 *
 * @param node Pointer to the current scene graph node.
 * @param transform The current transformation matrix.
 * @param lights Vector to store the collected lights.
 */
void collectLights(sgraph::SGNode *node, const glm::mat4 &transform,
                   vector<util::Light> &lights) {
  sgraph::AbstractSGNode *absNode =
      dynamic_cast<sgraph::AbstractSGNode *>(node);
  if (absNode) {
    for (const auto &l : absNode->getLights()) {
      util::Light tl = l;
      glm::vec4 pos = tl.getPosition();
      pos = transform * pos;
      tl.setPosition(pos);
      glm::vec3 spotDir =
          glm::mat3(transform) * glm::vec3(tl.getSpotDirection());
      tl.setSpotDirection(spotDir.x, spotDir.y, spotDir.z);
      lights.push_back(tl);
    }
  }
  sgraph::ParentSGNode *parentNode = dynamic_cast<sgraph::ParentSGNode *>(node);
  if (parentNode) {
    vector<sgraph::SGNode *> children = parentNode->getChildren();
    for (size_t i = 0; i < children.size(); i++) {
      sgraph::TransformNode *transNode =
          dynamic_cast<sgraph::TransformNode *>(children[i]);
      glm::mat4 childTransform = transform;
      if (transNode) {
        childTransform = transform * transNode->getTransform();
      }
      collectLights(children[i], childTransform, lights);
    }
  }
}

/**
 * @brief Initializes the View rendering context and scene resources.
 *
 * This method sets up GLFW, creates a window, initializes OpenGL via glad,
 * loads shaders, creates default white texture, sets up objects and textures,
 * and prepares the projection matrix.
 *
 * @param callbacks Pointer to the callbacks handler.
 * @param meshes Map of mesh names to their corresponding PolygonMesh data.
 * @param isTextRender Flag indicating if text rendering is required.
 * @param texturePaths Map of texture names to their file paths.
 */
void View::init(Callbacks *callbacks,
                map<string, util::PolygonMesh<VertexAttrib>> &meshes,
                bool isTextRender, map<string, string> texturePaths) {
  if (!glfwInit())
    exit(EXIT_FAILURE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  window = glfwCreateWindow(800, 800, "OpenGL Scene Graph Renderer", NULL, NULL);
  glfwSetWindowAspectRatio(window, 800, 800);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwSetWindowUserPointer(window, (void *)callbacks);
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
    reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))
        ->onkey(key, scancode, action, mods);
  });
  glfwSetMouseButtonCallback(
      window, [](GLFWwindow *window, int button, int action, int mods) {
        reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))
            ->onmousepress(button, action, mods);
      });
  glfwSetCursorPosCallback(
      window, [](GLFWwindow *window, double xpos, double ypos) {
        reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))
            ->onmousemove(xpos, ypos);
      });
  glfwSetWindowSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))
            ->reshape(width, height);
      });
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  // Create and setup phong shaders
  program.createProgram(string("shaders/phong-multiple.vert"),
                        string("shaders/phong-multiple.frag"));
  // Alternative shader program creation (commented out)
  // program.createProgram(string("shaders/default.vert"),
  //                       string("shaders/default.frag"));

  program.enable();
  shaderLocations = program.getAllShaderVariables();

  // Create default white texture
  glGenTextures(1, &defaultTexture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, defaultTexture);
  unsigned char white[4] = {255, 255, 255, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               white);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Bind the image texture to the shader
  glUniform1i(shaderLocations.getLocation("image"), 0);

  // Mapping shader variable names to vertex attribute names.
  map<string, string> shaderVarsToVertexAttribs;
  shaderVarsToVertexAttribs["vPosition"] = "position";
  shaderVarsToVertexAttribs["vNormal"] = "normal";
  shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";

  // Initialize objects from the provided meshes.
  for (auto it = meshes.begin(); it != meshes.end(); ++it) {
    util::ObjectInstance *obj = new util::ObjectInstance(it->first);
    obj->initPolygonMesh(shaderLocations, shaderVarsToVertexAttribs,
                         it->second);
    objects[it->first] = obj;
  }

  textures["white"] = defaultTexture;

  // Load model textures from given file paths.
  map<string, string> modelTexturePaths = texturePaths;

  for (auto const &entry : modelTexturePaths) {
    string texName = entry.first;
    string filePath = entry.second;
    PPMImageLoader loader;
    try {
      loader.load(filePath);
    } catch (exception &e) {
      cerr << "Error loading texture " << texName << " from " << filePath
           << ": " << e.what() << endl;
      continue;
    }
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, loader.getWidth(),
                 loader.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                 loader.getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    textures[texName] = texID;
  }

  // Setup the projection matrix based on the current frame buffer size.
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  projection = glm::perspective(
      glm::radians(60.0f), (float)window_width / window_height, 0.1f, 10000.0f);
  glViewport(0, 0, window_width, window_height);
  while (!modelview.empty())
    modelview.pop();
  modelview.push(glm::mat4(1.0f));

  // Initialize renderers if text rendering is disabled.
  if (!isTextRender) {
    renderer = new sgraph::GLScenegraphRenderer(
        modelview, objects, shaderLocations, textures, defaultTexture);
    rayRenderer =
        new sgraph::RaycastScenegraphRenderer(modelview, objects, 800, 800);
  }
}

/**
 * @brief Renders the scene graph on the screen.
 *
 * Clears the screen, updates the camera based on spherical coordinates,
 * applies shader parameters, collects lights from the scene graph,
 * and renders both the scene and optionally a raycast output.
 *
 * @param scenegraph Pointer to the scene graph to render.
 */
void View::display(sgraph::IScenegraph *scenegraph) {
  program.enable();
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  while (!modelview.empty())
    modelview.pop();
  modelview.push(glm::mat4(1.0f));

  // Compute camera position from spherical coordinates.
  float radius = 350.0f;                      // Camera distance from center.
  float radPitch = glm::radians(cameraPitch); // Pitch angle in radians.
  float radYaw = glm::radians(cameraYaw);     // Yaw angle in radians.

  // Calculate eye position on a sphere centered at (0,0,0)
  glm::vec3 eye;
  eye.x = radius * cos(radPitch) * sin(radYaw);
  eye.y = radius * sin(radPitch);
  eye.z = radius * cos(radPitch) * cos(radYaw);

  glm::vec3 center(0.0f, 0.0f, 0.0f);
  glm::vec3 up(0.0f, 1.0f, 0.0f);
  glm::mat4 viewMatrix = glm::lookAt(eye, center, up);
  modelview.top() = viewMatrix;

  // Set the projection matrix uniform.
  glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE,
                     glm::value_ptr(projection));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, defaultTexture);

  // Collect and send light parameters to the shader.
  vector<util::Light> lights;
  collectLights(scenegraph->getRoot(), modelview.top(), lights);
  int numLights = lights.size();
  glUniform1i(shaderLocations.getLocation("numLights"), numLights);
  for (int i = 0; i < numLights; i++) {
    string prefix = "light[" + to_string(i) + "].";
    glUniform3fv(shaderLocations.getLocation(prefix + "ambient"), 1,
                 glm::value_ptr(lights[i].getAmbient()));
    glUniform3fv(shaderLocations.getLocation(prefix + "diffuse"), 1,
                 glm::value_ptr(lights[i].getDiffuse()));
    glUniform3fv(shaderLocations.getLocation(prefix + "specular"), 1,
                 glm::value_ptr(lights[i].getSpecular()));
    glUniform4fv(shaderLocations.getLocation(prefix + "position"), 1,
                 glm::value_ptr(lights[i].getPosition()));
    glUniform3fv(shaderLocations.getLocation(prefix + "spotDirection"), 1,
                 glm::value_ptr(glm::vec3(lights[i].getSpotDirection())));
    glUniform1f(shaderLocations.getLocation(prefix + "spotCutoff"),
                lights[i].getSpotCutoff());
  }

  // Render the scene graph using the standard renderer.
  scenegraph->getRoot()->accept(renderer);
  // Render to output file if flag is set.
  if (shouldOutput) {
    shouldOutput = false;
    rayRenderer->render(scenegraph->getRoot(), "output.ppm");
  }
  glFlush();
  program.disable();
  glfwSwapBuffers(window);
  glfwPollEvents();
}

/**
 * @brief Checks if the window should be closed.
 *
 * @return true if the window should close, false otherwise.
 */
bool View::shouldWindowClose() { return glfwWindowShouldClose(window); }

/**
 * @brief Cleans up resources and closes the window.
 *
 * Iterates through all objects, calls cleanup, destroys the window,
 * and terminates GLFW.
 */
void View::closeWindow() {
  for (auto it = objects.begin(); it != objects.end(); it++) {
    it->second->cleanup();
    delete it->second;
  }
  glfwDestroyWindow(window);
  glfwTerminate();
}

/**
 * @brief Rotates the camera by modifying the pitch and yaw.
 *
 * @param pitch Amount to change the camera's pitch.
 * @param yaw Amount to change the camera's yaw.
 */
void View::rotateCamera(float pitch, float yaw) {
  cameraPitch += pitch;
  cameraYaw += yaw;
}

/**
 * @brief Resets the camera to its default orientation.
 *
 * Sets the camera pitch and yaw to predefined default values.
 */
void View::resetCamera() {
  cameraPitch = -35.264f;
  cameraYaw = -135.0f;
}
