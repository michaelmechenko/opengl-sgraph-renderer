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

class GLScenegraphRenderer : public SGNodeVisitor {
public:
  GLScenegraphRenderer(stack<glm::mat4> &mv,
                       map<string, util::ObjectInstance *> &os,
                       util::ShaderLocationsVault &shaderLocations,
                       map<string, GLuint> &textureMap, GLuint defaultTex)
      : modelview(mv), objects(os), textures(textureMap),
        defaultTexture(defaultTex) {
    this->shaderLocations = shaderLocations;
  }

  void visitGroupNode(GroupNode *groupNode) override {
    modelview.push(modelview.top());
    // multiply by the group's animation transform
    modelview.top() = modelview.top() * groupNode->getAnimTransform();
    vector<SGNode *> children = groupNode->getChildren();
    for (int i = 0; i < children.size(); i++) {
      children[i]->accept(this);
    }
    modelview.pop();
  }

  void visitLeafNode(LeafNode *leafNode) override {
    glm::mat4 currentMV = modelview.top();
    // set modelview uniform
    glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE,
                       glm::value_ptr(currentMV));
    // set normal matrix
    glm::mat4 normalMatrix = glm::inverse(glm::transpose(currentMV));
    glUniformMatrix4fv(shaderLocations.getLocation("normalmatrix"), 1, GL_FALSE,
                       glm::value_ptr(normalMatrix));
    // set texture matrix
    glm::mat4 textureMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(shaderLocations.getLocation("texturematrix"), 1,
                       GL_FALSE, glm::value_ptr(textureMatrix));

    util::Material mat = leafNode->getMaterial();
    glUniform3fv(shaderLocations.getLocation("material.ambient"), 1,
                 glm::value_ptr(mat.getAmbient()));
    glUniform3fv(shaderLocations.getLocation("material.diffuse"), 1,
                 glm::value_ptr(mat.getDiffuse()));
    glUniform3fv(shaderLocations.getLocation("material.specular"), 1,
                 glm::value_ptr(mat.getSpecular()));
    glUniform1f(shaderLocations.getLocation("material.shininess"),
                mat.getShininess());

    // bind texture if available
    string texName = leafNode->getTexture();
    glActiveTexture(GL_TEXTURE0);
    if ((texName != "") && (textures.find(texName) != textures.end())) {
      glBindTexture(GL_TEXTURE_2D, textures[texName]);
    } else {
      glBindTexture(GL_TEXTURE_2D, defaultTexture);
    }
    // draw object instance
    objects[leafNode->getInstanceOf()]->draw();
  }

  void visitTransformNode(TransformNode *transformNode) override {
    modelview.push(modelview.top());
    modelview.top() = modelview.top() * transformNode->getTransform();
    if (!transformNode->getChildren().empty())
      transformNode->getChildren()[0]->accept(this);
    modelview.pop();
  }

  void visitScaleTransform(ScaleTransform *scaleNode) override {
    visitTransformNode(scaleNode);
  }

  void visitTranslateTransform(TranslateTransform *translateNode) override {
    visitTransformNode(translateNode);
  }

  void visitRotateTransform(RotateTransform *rotateNode) override {
    visitTransformNode(rotateNode);
  }

private:
  stack<glm::mat4> &modelview;
  util::ShaderLocationsVault shaderLocations;
  map<string, util::ObjectInstance *> objects;
  map<string, GLuint> textures;
  GLuint defaultTexture;
};
} // namespace sgraph
#endif
