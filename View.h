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

class View {
public:
  View();
  ~View();
  void init(Callbacks *callbacks,
            map<string, util::PolygonMesh<VertexAttrib>> &meshes,
            bool isTextRender, map<string, string> texturePaths);
  void display(sgraph::IScenegraph *scenegraph);
  void rotateCamera(float pitch, float yaw);
  void resetCamera();
  bool shouldWindowClose();
  void closeWindow();
  static bool shouldOutput;

private:
  GLFWwindow *window;
  util::ShaderProgram program;
  util::ShaderLocationsVault shaderLocations;
  map<string, util::ObjectInstance *> objects;
  glm::mat4 projection;
  stack<glm::mat4> modelview;
  sgraph::RaycastScenegraphRenderer *rayRenderer;
  sgraph::GLScenegraphRenderer *renderer;

  map<string, GLuint> textures;
  GLuint defaultTexture;
  bool isTextRender;
};
#endif
