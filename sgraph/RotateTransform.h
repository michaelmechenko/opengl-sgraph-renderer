#ifndef _ROTATETRANSFORM_H_
#define _ROTATETRANSFORM_H_

#include "TransformNode.h"
#include <glm/gtc/matrix_transform.hpp>

namespace sgraph {

/**
 * This class represents a specific type of transformation: rotation
 *
 */
class RotateTransform : public TransformNode {
protected:
  float angleInRadians;
  glm::vec3 axis;

  ParentSGNode *copyNode() {
    return new RotateTransform(angleInRadians, axis[0], axis[1], axis[2], name,
                               scenegraph);
  }

public:
  RotateTransform(float angleInRadians, float ax, float ay, float az,
                  const string &name, sgraph::IScenegraph *graph)
      : TransformNode(name, graph) {
    this->angleInRadians = angleInRadians;
    this->axis = glm::vec3(ax, ay, az);
    glm::mat4 transform =
        glm::rotate(glm::mat4(1.0), this->angleInRadians, this->axis);
    setTransform(transform);
  }

  /**
   * Visit this node.
   *
   */
  void accept(SGNodeVisitor *visitor) {
    return visitor->visitRotateTransform(this);
  }

  glm::vec3 getRotationAxis() { return axis; }

  float getAngleInRadians() { return angleInRadians; }
};
} // namespace sgraph

#endif
