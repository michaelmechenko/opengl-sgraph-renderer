#ifndef _ANIMATEDNODEVISITOR_H_
#define _ANIMATEDNODEVISITOR_H_

#include "GroupNode.h"
#include "SGNodeVisitor.h"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace sgraph {

/**
 * @brief A visitor class for traversing and animating scene graph nodes.
 *
 * This class implements the SGNodeVisitor interface to traverse different
 * types of scene graph nodes and update animations based on the elapsed time.
 */
class AnimationVisitor : public SGNodeVisitor {
public:
  /**
   * @brief Constructs an AnimationVisitor with a given time delta.
   *
   * @param dt The elapsed time since the last update.
   */
  AnimationVisitor(double dt) : deltaTime(dt) {}

  /**
   * @brief Visits a GroupNode to update animations for its children.
   *
   * This method iterates over all the children of the GroupNode and applies
   * the animation visitor to each child recursively.
   *
   * @param node Pointer to the GroupNode.
   */
  virtual void visitGroupNode(GroupNode *node) {
    for (auto child : node->getChildren()) {
      child->accept(this);
    }
  }

  /**
   * @brief Visits a LeafNode.
   *
   * Leaf nodes do not have animations to update, so this function performs no
   * actions.
   *
   * @param node Pointer to the LeafNode.
   */
  virtual void visitLeafNode(LeafNode *node) {
    // Nothing to update for leaf nodes
  }

  /**
   * @brief Visits a TransformNode.
   *
   * This function handles transformation nodes. The specific update logic for
   * transform nodes can be customized. The commented code shows an example of
   * how to process a child of a transform node.
   *
   * @param node Pointer to the TransformNode.
   */
  virtual void visitTransformNode(TransformNode *node) {
    // The following code demonstrates how to visit children of the transform
    // node: if (!node->getChildren().empty()) {
    //   node->getChildren()[0]->accept(this);
    // }
  }

  /**
   * @brief Visits a ScaleTransform node.
   *
   * Currently, this function defers processing to the visitTransformNode
   * function.
   *
   * @param node Pointer to the ScaleTransform node.
   */
  virtual void visitScaleTransform(ScaleTransform *node) {
    // Process scale transform node, potentially using visitTransformNode(node);
    // visitTransformNode(node);
  }

  /**
   * @brief Visits a TranslateTransform node.
   *
   * Currently, this function defers processing to the visitTransformNode
   * function.
   *
   * @param node Pointer to the TranslateTransform node.
   */
  virtual void visitTranslateTransform(TranslateTransform *node) {
    // Process translate transform node, potentially using
    // visitTransformNode(node); visitTransformNode(node);
  }

  /**
   * @brief Visits a RotateTransform node.
   *
   * This function handles animated rotation transformations. The code
   * demonstrates how to update the animation for a rotation transform if it
   * is an instance of AnimatedRotateTransform.
   *
   * @param node Pointer to the RotateTransform node.
   */
  virtual void visitRotateTransform(RotateTransform *node) {
    // Example implementation:
    // AnimatedRotateTransform *arot =
    //     dynamic_cast<AnimatedRotateTransform *>(node);
    // if (arot)
    //   arot->updateAnimation(deltaTime);
    // visitTransformNode(node);
  }

private:
  double deltaTime; // Elapsed time since last update, used for animations.
};

} // namespace sgraph
#endif
