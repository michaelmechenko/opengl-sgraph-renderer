#ifndef _ANIMATEDNODEVISITOR_H_
#define _ANIMATEDNODEVISITOR_H_

#include "GroupNode.h"
#include "SGNodeVisitor.h"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace sgraph {

class AnimationVisitor : public SGNodeVisitor {
public:
  AnimationVisitor(double dt) : deltaTime(dt) {}

  virtual void visitGroupNode(GroupNode *node) {
    for (auto child : node->getChildren()) {
      child->accept(this);
    }
  }
  virtual void visitLeafNode(LeafNode *node) {
    // nothing to update for leaves 
  }
  virtual void visitTransformNode(TransformNode *node) {
    // if (!node->getChildren().empty()) {
    //   node->getChildren()[0]->accept(this);
    // }
  }
  virtual void visitScaleTransform(ScaleTransform *node) {
    // visitTransformNode(node);
  }
  virtual void visitTranslateTransform(TranslateTransform *node) {
    // visitTransformNode(node);
  }
  virtual void visitRotateTransform(RotateTransform *node) {
    // AnimatedRotateTransform *arot =
    //     dynamic_cast<AnimatedRotateTransform *>(node);
    // if (arot)
    //   arot->updateAnimation(deltaTime);
    // visitTransformNode(node);
  }

private:
  double deltaTime; // elapsed time since last update
};
} // namespace sgraph
#endif
