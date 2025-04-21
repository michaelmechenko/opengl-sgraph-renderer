#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "IScenegraph.h"
#include "IVertexData.h"
#include "PolygonMesh.h"
#include "SGNode.h"
#include "glm/glm.hpp"
#include <map>
#include <string>
using namespace std;

namespace sgraph {

/**
 * A specific implementation of this scene graph. This implementation is still
 * independent of the rendering technology (i.e. OpenGL) \author Amit Shesh
 */

class Scenegraph : public IScenegraph {
  /**
   * The root of the scene graph tree
   */
protected:
  SGNode *root;
  map<string, util::PolygonMesh<VertexAttrib>> meshes;
  map<string, string> meshPaths;

  /**
   * A map to store the (name,node) pairs. A map is chosen for efficient search
   */
  map<string, SGNode *> nodes;

public:
  Scenegraph() { root = NULL; }

  ~Scenegraph() { dispose(); }

  void dispose() {

    if (root != NULL) {
      delete root;
      root = NULL;
    }
  }

  /**
   * Set the root of the scenegraph, and then pass a reference to this scene
   * graph object to all its node. This will enable any node to call functions
   * of its associated scene graph \param root
   */

  void makeScenegraph(SGNode *root) {
    this->root = root;
    if (root != NULL) {
      this->root->setScenegraph(this);
    }
  }

  void addNode(const string &name, SGNode *node) { nodes[name] = node; }

  SGNode *getRoot() { return root; }

  map<string, SGNode *> getNodes() { return nodes; }

  void setMeshes(map<string, util::PolygonMesh<VertexAttrib>> &meshes) {
    this->meshes = meshes;
  }

  map<string, util::PolygonMesh<VertexAttrib>> getMeshes() {
    return this->meshes;
  }

  void setMeshPaths(map<string, string> &meshPaths) {
    this->meshPaths = meshPaths;
  }

  map<string, string> getMeshPaths() { return this->meshPaths; }
};
} // namespace sgraph
#endif
