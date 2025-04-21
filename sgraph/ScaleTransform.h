#ifndef _SCALETRANSFORM_H_
#define _SCALETRANSFORM_H_

#include "IScenegraph.h"
#include "SGNode.h"
#include "TransformNode.h"
#include <glm/gtc/matrix_transform.hpp>

namespace sgraph {
/**
 * This node represents a specific type of transform: scale
 *
 */
class ScaleTransform : public TransformNode {
protected:
  float sx, sy, sz;

  ParentSGNode *copyNode() {
    return new ScaleTransform(sx, sy, sz, name, scenegraph);
  }

public:
  ScaleTransform(float sx, float sy, float sz, const string &name,
                 sgraph::IScenegraph *graph)
      : TransformNode(name, graph) {
    this->sx = sx;
    this->sy = sy;
    this->sz = sz;
    glm::mat4 transform = glm::scale(glm::mat4(1.0), glm::vec3(sx, sy, sz));
    setTransform(transform);
  }

  /**
   * Visit this node.
   *
   */
  void accept(SGNodeVisitor *visitor) {
    return visitor->visitScaleTransform(this);
  }

  glm::vec3 getScale() { return glm::vec3(sx, sy, sz); }
};
} // namespace sgraph

#endif
