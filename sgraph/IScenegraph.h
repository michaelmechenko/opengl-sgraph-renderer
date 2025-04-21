#ifndef _ISCENEGRAPH_H_
#define _ISCENEGRAPH_H_

#include "IVertexData.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "SGNode.h"
#include <glm/glm.hpp>

#include <map>
#include <stack>
using namespace std;
namespace sgraph {

/**
 * This virtual class captures all the operations that a scene graph should
 * offer. It is designed to be a generic scene graph that is independent of the
 * actual rendering library.
 *
 * The scene graph also stores references to all the nodes keyed by
 * their name. This way the scene graph can directly refer to any of its nodes
 * by name instead of traversing the tree every time to find it. This is useful
 * when nodes must be identified and animated in specific ways.
 * \author Amit Shesh
 */

class IScenegraph {
public:
  virtual ~IScenegraph() {}
  /**
   * initialize the supplied root to the be the root of this scene graph.
   * This is supposed to overwrite any previous roots
   * \param root
   */
  virtual void makeScenegraph(SGNode *root) = 0;

  /**
   * Adds a node to itself. This should be stored in a suitable manner by an
   * implementation, so that it is possible to look up a specific node by name
   * \param name (hopefully unique) name given to this node
   * \param node the node object
   */
  virtual void addNode(const string &name, SGNode *node) = 0;

  /**
   * Get the root of this scene graph
   * \return the root of this scene graph
   */
  virtual SGNode *getRoot() = 0;

  /**
   * Get a mapping of all (name,INode) pairs for all nodes in this scene graph.
   * This function is useful in case all meshes of one scene graph have to be
   * added to another in an attempt to merge two scene graphs
   */
  virtual map<string, SGNode *> getNodes() = 0;
  virtual void dispose() = 0;

  /**
   * Set the meshes used by this scene graph
   *
   * @param meshes
   */
  virtual void
  setMeshes(map<string, util::PolygonMesh<VertexAttrib>> &meshes) = 0;

  /**
   * Set the mesh name ->mesh path for all meshes used by this scene graph
   *
   * @param meshPaths
   */
  virtual void setMeshPaths(map<string, string> &meshPaths) = 0;

  /**
   * Get the meshes used by this scene graph
   *
   * @return map<string,util::PolygonMesh<VertexAttrib> >
   */
  virtual map<string, util::PolygonMesh<VertexAttrib>> getMeshes() = 0;

  /**
   * Get a map of each mesh name (as the leaves refer to it) and the path to the
   * mesh file
   *
   *
   * @return map<string,string>
   */
  virtual map<string, string> getMeshPaths() = 0;
};
} // namespace sgraph

#endif
