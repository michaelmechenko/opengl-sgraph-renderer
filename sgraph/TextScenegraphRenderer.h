#ifndef _GLSCENEGRAPHRENDERERDEPTH_H_
#define _GLSCENEGRAPHRENDERERDEPTH_H_

#include "GroupNode.h"
#include "LeafNode.h"
#include "ObjectInstance.h"
#include "RotateTransform.h"
#include "SGNodeVisitor.h"
#include "ScaleTransform.h"
#include "TransformNode.h"
#include "TranslateTransform.h"
#include <ShaderLocationsVault.h>
#include <ShaderProgram.h>
#include <iostream>
#include <stack>
using namespace std;

namespace sgraph {
/**
 * This visitor implements drawing the scene graph using OpenGL
 *
 */
class TextScenegraphRenderer : public SGNodeVisitorDepth {
public:
  /**
   * @brief Construct a new GLScenegraphRenderer object
   *
   * @param mv a reference to modelview stack that will be used while rendering
   * @param os the map of ObjectInstance objects
   * @param shaderLocations the shader locations for the program used to render
   */
  TextScenegraphRenderer(stack<glm::mat4> &mv,
                         map<string, util::ObjectInstance *> &os,
                         util::ShaderLocationsVault &shaderLocations)
      : modelview(mv), objects(os) {
    this->shaderLocations = shaderLocations;
  }

  void printNodeHelper(string nodeName, int nodeDepth) {
    for (int i = nodeDepth - 1; i > 0; i--) {
      cout << "    ";
    }
    cout << "- " << nodeName << endl;
  }

  /**
   * @brief Recur to the children for drawing
   *
   * @param groupNode
   */
  void visitGroupNode(GroupNode *groupNode, int depth) {
    printNodeHelper(groupNode->getName(), depth);
    for (int i = 0; i < groupNode->getChildren().size(); i = i + 1) {
      groupNode->getChildren()[i]->accept(this, depth);
    }
  }

  /**
   * @brief Draw the instance for the leaf, after passing the
   * modelview and color to the shader
   *
   * @param leafNode
   */
  void visitLeafNode(LeafNode *leafNode, int depth) {
    printNodeHelper(leafNode->getName(), depth);
  }

  /**
   * @brief Multiply the transform to the modelview and recur to child
   *
   * @param transformNode
   */
  void visitTransformNode(TransformNode *transformNode, int depth) {
    printNodeHelper(transformNode->getName(), depth);
    modelview.push(modelview.top());
    modelview.top() = modelview.top() * transformNode->getTransform();
    if (transformNode->getChildren().size() > 0) {
      transformNode->getChildren()[0]->accept(this, depth);
    }
    modelview.pop();
  }

  /**
   * @brief For this visitor, only the transformation matrix is required.
   * Thus there is nothing special to be done for each type of transformation.
   * We delegate to visitTransformNode above
   *
   * @param scaleNode
   */
  void visitScaleTransform(ScaleTransform *scaleNode, int depth) {
    visitTransformNode(scaleNode, depth);
  }

  /**
   * @brief For this visitor, only the transformation matrix is required.
   * Thus there is nothing special to be done for each type of transformation.
   * We delegate to visitTransformNode above
   *
   * @param translateNode
   */
  void visitTranslateTransform(TranslateTransform *translateNode, int depth) {
    visitTransformNode(translateNode, depth);
  }

  void visitRotateTransform(RotateTransform *rotateNode, int depth) {
    visitTransformNode(rotateNode, depth);
  }

private:
  stack<glm::mat4> &modelview;
  util::ShaderLocationsVault shaderLocations;
  map<string, util::ObjectInstance *> objects;
};
} // namespace sgraph

#endif
