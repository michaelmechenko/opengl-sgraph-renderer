
#ifndef _GLSCENEGRAPHRENDERER_H_
#define _GLSCENEGRAPHRENDERER_H_

#include "GroupNode.h"
#include "LeafNode.h"
#include "ObjectInstance.h"
#include "RotateTransform.h"
#include "SGNodeVisitor.h"
#include "ScaleTransform.h"
#include "TransformNode.h"
#include "TranslateTransform.h"
#include "glm/gtc/type_ptr.hpp"
#include <ShaderLocationsVault.h>
#include <ShaderProgram.h>
#include <iostream>
#include <stack>
using namespace std;

namespace sgraph {

/**
 * @brief Renderer for scene graphs using OpenGL.
 *
 * This class implements the SGNodeVisitor interface to traverse and render a
 * scene graph. It manages transformations via a modelview matrix stack and
 * renders objects with respect to lighting and materials.
 */
class GLScenegraphRenderer : public SGNodeVisitor {
public:
  /**
   * @brief Constructor for the GLScenegraphRenderer.
   *
   * @param mv           Reference to the modelview matrix stack.
   * @param os           Reference to a map of object instances.
   * @param shaderLocations  ShaderLocationsVault containing uniform locations.
   * @param textureMap   Reference to a map of textures.
   * @param defaultTex   Default texture ID to use if a texture is not found.
   */
  GLScenegraphRenderer(stack<glm::mat4> &mv,
                       map<string, util::ObjectInstance *> &os,
                       util::ShaderLocationsVault &shaderLocations,
                       map<string, GLuint> &textureMap, GLuint defaultTex)
      : modelview(mv), objects(os), textures(textureMap),
        defaultTexture(defaultTex) {
    this->shaderLocations = shaderLocations;
  }

  /**
   * @brief Visits a GroupNode and renders its content.
   *
   * Applies the group node's animation transform, recursively visits all
   * children, and then restores the previous modelview matrix.
   *
   * @param groupNode Pointer to the GroupNode to be visited.
   */
  void visitGroupNode(GroupNode *groupNode) override {
    // Push a copy of the current modelview matrix on the stack.
    modelview.push(modelview.top());
    // Multiply the top matrix by the group's animation transform.
    modelview.top() = modelview.top() * groupNode->getAnimTransform();
    // Visit all child nodes.
    vector<SGNode *> children = groupNode->getChildren();
    for (int i = 0; i < children.size(); i++) {
      children[i]->accept(this);
    }
    // Restore the previous modelview matrix.
    modelview.pop();
  }

  /**
   * @brief Visits a LeafNode and renders its associated object.
   *
   * Sets up shader uniforms for transformations, normals, textures, and
   * material properties, then binds the appropriate texture and invokes the
   * draw call on the object instance.
   *
   * @param leafNode Pointer to the LeafNode to be visited.
   */
  void visitLeafNode(LeafNode *leafNode) override {
    // Get the current modelview matrix.
    glm::mat4 currentMV = modelview.top();

    // Set the modelview uniform.
    glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE,
                       glm::value_ptr(currentMV));

    // Calculate and set the normal matrix.
    glm::mat4 normalMatrix = glm::inverse(glm::transpose(currentMV));
    glUniformMatrix4fv(shaderLocations.getLocation("normalmatrix"), 1, GL_FALSE,
                       glm::value_ptr(normalMatrix));

    // Create and set the texture matrix (identity matrix).
    glm::mat4 textureMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(shaderLocations.getLocation("texturematrix"), 1,
                       GL_FALSE, glm::value_ptr(textureMatrix));

    // Set material properties uniforms.
    util::Material mat = leafNode->getMaterial();
    glUniform3fv(shaderLocations.getLocation("material.ambient"), 1,
                 glm::value_ptr(mat.getAmbient()));
    glUniform3fv(shaderLocations.getLocation("material.diffuse"), 1,
                 glm::value_ptr(mat.getDiffuse()));
    glUniform3fv(shaderLocations.getLocation("material.specular"), 1,
                 glm::value_ptr(mat.getSpecular()));
    glUniform1f(shaderLocations.getLocation("material.shininess"),
                mat.getShininess());

    // Bind the appropriate texture.
    string texName = leafNode->getTexture();
    glActiveTexture(GL_TEXTURE0);
    if ((texName != "") && (textures.find(texName) != textures.end())) {
      glBindTexture(GL_TEXTURE_2D, textures[texName]);
    } else {
      glBindTexture(GL_TEXTURE_2D, defaultTexture);
    }

    // Draw the associated object instance.
    objects[leafNode->getInstanceOf()]->draw();
  }

  /**
   * @brief Visits a TransformNode and applies its transformation.
   *
   * Pushes the current modelview matrix, applies the node's transformation,
   * visits its first child (if available), then restores the matrix.
   *
   * @param transformNode Pointer to the TransformNode to be visited.
   */
  void visitTransformNode(TransformNode *transformNode) override {
    // Save the current state.
    modelview.push(modelview.top());
    // Multiply current matrix by the node's transformation.
    modelview.top() = modelview.top() * transformNode->getTransform();
    // Visit the first child if it exists.
    if (!transformNode->getChildren().empty())
      transformNode->getChildren()[0]->accept(this);
    // Restore the previous modelview matrix.
    modelview.pop();
  }

  /**
   * @brief Visits a ScaleTransform node.
   *
   * Delegates processing to visitTransformNode as scale is a type of transform.
   *
   * @param scaleNode Pointer to the ScaleTransform node.
   */
  void visitScaleTransform(ScaleTransform *scaleNode) override {
    visitTransformNode(scaleNode);
  }

  /**
   * @brief Visits a TranslateTransform node.
   *
   * Delegates processing to visitTransformNode as translation is a type of
   * transform.
   *
   * @param translateNode Pointer to the TranslateTransform node.
   */
  void visitTranslateTransform(TranslateTransform *translateNode) override {
    visitTransformNode(translateNode);
  }

  /**
   * @brief Visits a RotateTransform node.
   *
   * Delegates processing to visitTransformNode as rotation is a type of
   * transform.
   *
   * @param rotateNode Pointer to the RotateTransform node.
   */
  void visitRotateTransform(RotateTransform *rotateNode) override {
    visitTransformNode(rotateNode);
  }

private:
  // Reference to the modelview matrix stack used for transformations.
  stack<glm::mat4> &modelview;
  // Shader uniform locations.
  util::ShaderLocationsVault shaderLocations;
  // Map of object instances used in the scene.
  map<string, util::ObjectInstance *> objects;
  // Map of textures identified by name.
  map<string, GLuint> textures;
  // Default texture to use if no texture is provided.
  GLuint defaultTexture;
};

} // namespace sgraph
#endif
