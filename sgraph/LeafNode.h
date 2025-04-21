#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractSGNode.h"
#include "Material.h"
#include "SGNodeVisitor.h"
#include "SGNodeVisitorDepth.h"
#include "glm/glm.hpp"
#include <map>
#include <stack>
#include <string>
using namespace std;

namespace sgraph {

/**
 * This node represents the leaf of a scene graph. It is the only type of node
 * that has actual geometry to render. \author Amit Shesh
 */
class LeafNode : public AbstractSGNode {
  /**
   * The name of the object instance that this leaf contains. All object
   * instances are stored in the scene graph itself, so that an instance can be
   * reused in several leaves
   */
protected:
  string objInstanceName;
  /**
   * The material associated with the object instance at this leaf
   */
  util::Material material;
  string textureName; // stores name of the texture to use

public:
  LeafNode(const string &instanceOf, util::Material &material,
           const string &name, sgraph::IScenegraph *graph)
      : AbstractSGNode(name, graph) {
    this->objInstanceName = instanceOf;
    this->material = material;
    textureName = "";
  }

  LeafNode(const string &instanceOf, const string &name,
           sgraph::IScenegraph *graph)
      : AbstractSGNode(name, graph) {
    this->objInstanceName = instanceOf;
    textureName = "";
  }

  ~LeafNode() {}

  /*
   *Set the material of each vertex in this object
   */
  void setMaterial(const util::Material &mat) { material = mat; }

  /*
   * gets the material
   */
  util::Material getMaterial() { return material; }

  /**
   * Get the name of the instance this leaf contains
   *
   * @return string
   */
  string getInstanceOf() { return this->objInstanceName; }

  /**
   * Get a copy of this node.
   *
   * @return SGNode*
   */

  SGNode *clone() {
    LeafNode *newclone =
        new LeafNode(this->objInstanceName, material, name, scenegraph);
    return newclone;
  }

  /**
   * Visit this node.
   *
   */
  void accept(SGNodeVisitor *visitor) { visitor->visitLeafNode(this); }
  void accept(SGNodeVisitorDepth *visitor, int depth) {
    visitor->visitLeafNode(this, depth + 1);
  }

  void setTexture(const string &tex) { textureName = tex; }
  string getTexture() { return textureName; }
};
} // namespace sgraph
#endif
