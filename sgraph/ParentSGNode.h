#ifndef _PARENTSGNODE_H_
#define _PARENTSGNODE_H_

#include "AbstractSGNode.h"
#include <glm/glm.hpp>
using namespace std;
namespace sgraph {

class ParentSGNode : public AbstractSGNode {
public:
  ParentSGNode(const string &name, IScenegraph *scenegraph)
      : AbstractSGNode(name, scenegraph), animTransform(glm::mat4(1.0f)) {}

  ~ParentSGNode() {
    for (int i = 0; i < children.size(); i++) {
      delete children[i];
    }
  }

  virtual void addChild(SGNode *child) = 0;
  vector<SGNode *> getChildren() { return children; }

  SGNode *getNode(const string &name) {
    SGNode *n = AbstractSGNode::getNode(name);
    if (n != NULL)
      return n;
    int i = 0;
    SGNode *answer = NULL;
    while ((i < children.size()) && (answer == NULL)) {
      answer = children[i]->getNode(name);
      i++;
    }
    return answer;
  }

  SGNode *clone() {
    ParentSGNode *newtransform = copyNode();
    for (int i = 0; i < children.size(); i++) {
      newtransform->addChild(children[i]->clone());
    }
    return newtransform;
  }

  void setAnimTransform(const glm::mat4 &m) { animTransform = m; }
  glm::mat4 getAnimTransform() const { return animTransform; }

protected:
  vector<SGNode *> children;
  glm::mat4 animTransform;
  virtual ParentSGNode *copyNode() = 0;
};
} // namespace sgraph
#endif
