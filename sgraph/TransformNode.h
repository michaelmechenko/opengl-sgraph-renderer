#ifndef _TRANSFORMNODE_H_
#define _TRANSFORMNODE_H_

#include "ParentSGNode.h"
#include "SGNodeVisitor.h"
#include "glm/glm.hpp"
#include <string>
#include <vector>
using namespace std;

namespace sgraph {

/**
 * This node represents a transformation in the scene graph. It has only one
 * child. The transformation can be viewed as changing from its child's
 * coordinate system to its parent's coordinate system
 * \author Amit Shesh
 */
class TransformNode : public ParentSGNode {
protected:
  glm::mat4 transform;

  void setTransform(glm::mat4 &transform) { this->transform = transform; }

public:
  TransformNode(const string &name, sgraph::IScenegraph *graph)
      : ParentSGNode(name, graph) {
    this->transform = glm::mat4(1.0);
  }

  ~TransformNode() {}

  /**
   * Since this node can have a child, it override this method and adds the
   * child to itself This will overwrite any children set for this node
   * previously.
   * \param child the child of this node
   * \throws runtime_error if a child already exists
   */
  void addChild(SGNode *child) {
    if (this->children.size() > 0)
      throw runtime_error("Transform node already has a child");
    this->children.push_back(child);
    child->setParent(this);
  }

  /**
   * Gets the transform at this node (not the animation transform)
   */
  glm::mat4 getTransform() { return transform; }

  /**
   * Sets the scene graph object of which this node is a part, and then recurses
   * to its child
   * \param graph a reference to the scenegraph object of which this tree is a
   * part
   */
  void setScenegraph(sgraph::IScenegraph *graph) {
    AbstractSGNode::setScenegraph(graph);
    if (children.size() > 0) {
      children[0]->setScenegraph(graph);
    }
  }

  /**
   * Visit this node.
   *
   */
  void accept(SGNodeVisitor *visitor) {
    return visitor->visitTransformNode(this);
  }
  void accept(SGNodeVisitorDepth *visitor, int depth) {
    return visitor->visitTransformNode(this, depth + 1);
  }
};
} // namespace sgraph
#endif
