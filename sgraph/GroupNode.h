#ifndef _GROUPNODE_H_
#define _GROUPNODE_H_

#include "ParentSGNode.h"
#include "SGNodeVisitor.h"
#include "SGNodeVisitorDepth.h"
#include "glm/glm.hpp"
#include <stack>
#include <string>
#include <vector>
using namespace std;

namespace sgraph {
/**
 * This class represents a group node in the scenegraph. A group node is simply
 * a logical grouping of other nodes. It can have an arbitrary number of
 * children. Its children can be nodes of any type
 * \author Amit Shesh
 */
class GroupNode : public ParentSGNode {

protected:
  ParentSGNode *copyNode() { return new GroupNode(name, scenegraph); }

public:
  GroupNode(const string &name, sgraph::IScenegraph *graph)
      : ParentSGNode(name, graph) {}

  ~GroupNode() {}

  /**
   * Sets the reference to the scene graph object for this node, and then
   * recurses down to children for the same
   * \param graph a reference to the scenegraph object of which this tree is a
   * part
   */
  void setScenegraph(sgraph::IScenegraph *graph) {
    AbstractSGNode::setScenegraph(graph);
    for (int i = 0; i < children.size(); i++) {
      children[i]->setScenegraph(graph);
    }
  }

  /**
   * Makes a deep copy of the subtree rooted at this node
   * \return a deep copy of the subtree rooted at this node
   */
  SGNode *clone() {
    vector<SGNode *> newc;

    for (int i = 0; i < children.size(); i++) {
      newc.push_back(children[i]->clone());
    }

    GroupNode *newgroup = new GroupNode(name, scenegraph);

    for (int i = 0; i < children.size(); i++) {
      try {
        newgroup->addChild(newc[i]);
      } catch (runtime_error e) {
      }
    }
    return newgroup;
  }

  /**
   * Since a group node is capable of having children, this method overrides the
   * default one in sgraph::AbstractNode and adds a child to this node
   * \param child
   * \throws runtime_error this class does not throw this exception
   */
  void addChild(SGNode *child) {
    children.push_back(child);
    child->setParent(this);
  }

  /**
   * Visit this node.
   *
   */
  void accept(SGNodeVisitor *visitor) { visitor->visitGroupNode(this); }
  void accept(SGNodeVisitorDepth *visitor, int depth) {
    visitor->visitGroupNode(this, depth + 1);
  }
};
} // namespace sgraph

#endif
