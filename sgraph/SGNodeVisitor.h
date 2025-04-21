#ifndef _SGNODEVISITOR_H_
#define _SGNODEVISITOR_H_

#include <string>
using namespace std;

/*
#include "GroupNode.h"
#include "LeafNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TransformNode.h"
#include "TranslateTransform.h"
*/

namespace sgraph {
class GroupNode;
class LeafNode;
class TransformNode;
class ScaleTransform;
class RotateTransform;
class TranslateTransform;

/**
 * This class represents the interface for a visitor on scene graph nodes.
 * The visitor design pattern ensures that one can implement various
 * functions on the scene graph nodes, while not having to change the nodes.
 *
 *
 */
class SGNodeVisitor {
public:
  virtual void visitGroupNode(GroupNode *node) = 0;
  virtual void visitLeafNode(LeafNode *node) = 0;
  virtual void visitTransformNode(TransformNode *node) = 0;
  virtual void visitScaleTransform(ScaleTransform *node) = 0;
  virtual void visitTranslateTransform(TranslateTransform *node) = 0;
  virtual void visitRotateTransform(RotateTransform *node) = 0;
  // virtual void visitAnimatedTransformNode(TransformNode *node) = 0;
  // virtual void visitAnimatedScaleTransform(ScaleTransform *node) = 0;
  // virtual void visitAnimatedTranslateTransform(TranslateTransform *node) = 0;
  // virtual void visitAnimatedRotateTransform(RotateTransform *node) = 0;
};
} // namespace sgraph

#endif
