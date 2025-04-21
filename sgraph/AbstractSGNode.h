#ifndef _ABSTRACTSGNODE_H_
#define _ABSTRACTSGNODE_H_

#include "IScenegraph.h"
#include "Light.h"
#include "SGNode.h"
#include "glm/glm.hpp"
#include <string>
using namespace std;

namespace sgraph {

/**
 * This abstract class implements the sgraph::SGNode interface.
 * \author Amit Shesh
 */
class AbstractSGNode : public SGNode {
  /**
   * The name given to this node
   */
protected:
  string name;
  /**
   * The parent of this node. Each node except the root has a parent. The root's
   * parent is null
   */
  SGNode *parent;
  /**
   * A reference to the sgraph::IScenegraph object that this is part of
   */
  sgraph::IScenegraph *scenegraph;

  // holds lights/node
  vector<util::Light> nodeLights;

public:
  AbstractSGNode(const string &name, sgraph::IScenegraph *graph) {
    this->parent = NULL;
    scenegraph = graph;
    setName(name);
  }

  /**
   * By default, this method checks only itself. Nodes that have children should
   * override this method and navigate to children to find the one with the
   * correct name
   * \param name name of node to be searched
   * \return the node whose name this is, null otherwise
   */
  SGNode *getNode(const string &name) {
    if (this->name == name)
      return this;

    return NULL;
  }

  /**
   * Sets the parent of this node
   * \param parent the node that is to be the parent of this node
   */

  void setParent(SGNode *parent) { this->parent = parent; }

  /**
   * Sets the scene graph object whose part this node is and then adds itself
   * to the scenegraph (in case the scene graph ever needs to directly access
   * this node)
   * \param graph a reference to the scenegraph object of which this tree is a
   * part
   */
  void setScenegraph(sgraph::IScenegraph *graph) {
    this->scenegraph = graph;
    graph->addNode(this->name, this);
  }

  /**
   * Sets the name of this node
   * \param name the name of this node
   */
  void setName(const string &name) { this->name = name; }

  /**
   * Gets the name of this node
   * \return the name of this node
   */
  string getName() { return name; }

  void addLight(const util::Light &l) { nodeLights.push_back(l); }
  const vector<util::Light> &getLights() const { return nodeLights; }
};
} // namespace sgraph
#endif
