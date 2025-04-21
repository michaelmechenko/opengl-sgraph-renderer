#ifndef _PARENTSGNODE_H_
#define _PARENTSGNODE_H_

// ParentSGNode.h
// This file defines the abstract ParentSGNode class which manages child nodes
// in a scene graph. It extends AbstractSGNode and provides functionality for
// cloning nodes and applying animation transformations.
// Author: [Your Name]
// Date: [Today's Date]

#include "AbstractSGNode.h" // Base class definition for scene graph nodes
#include <glm/glm.hpp>      // GLM library for matrix operations
#include <string>           // Standard string class
#include <vector>           // Standard vector container

// Bring common standard types into the global namespace for convenience.
using namespace std;

// Define the sgraph namespace to encapsulate scene graph related classes.
namespace sgraph {

/**
 * @class ParentSGNode
 * @brief Abstract class for parent nodes in a scene graph.
 *
 * This class inherits from AbstractSGNode and provides additional functionality
 * to manage child scene graph nodes. It also supports cloning of nodes and
 * storing an animation transformation matrix.
 */
class ParentSGNode : public AbstractSGNode {
public:
  /**
   * @brief Constructor for ParentSGNode.
   *
   * Initializes a ParentSGNode with a given name and associated scenegraph.
   * Also initializes the animation transformation matrix to the identity
   * matrix.
   *
   * @param name The name of the node.
   * @param scenegraph Pointer to the scene graph object.
   */
  ParentSGNode(const string &name, IScenegraph *scenegraph)
      : AbstractSGNode(name, scenegraph), animTransform(glm::mat4(1.0f)) {}

  /**
   * @brief Destructor for ParentSGNode.
   *
   * Deletes all child nodes to free allocated memory.
   */
  ~ParentSGNode() {
    for (int i = 0; i < children.size(); i++) {
      delete children[i];
    }
  }

  /**
   * @brief Pure virtual function to add a child node.
   *
   * Derived classes must implement this function to specify how a child
   * node is added to the list of children.
   *
   * @param child Pointer to the child scene graph node to add.
   */
  virtual void addChild(SGNode *child) = 0;

  /**
   * @brief Retrieve the list of child nodes.
   *
   * @return A vector of pointers to the child nodes.
   */
  vector<SGNode *> getChildren() { return children; }

  /**
   * @brief Retrieve a node by its name.
   *
   * Searches through this node and its children recursively to find the node
   * with the specified name.
   *
   * @param name The name of the node to search for.
   * @return Pointer to the node if found, otherwise NULL.
   */
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

  /**
   * @brief Clone the node along with its children.
   *
   * Creates a copy of the node (using copyNode) and then clones each child
   * recursively, adding them to the new node.
   *
   * @return Pointer to the cloned scene graph node.
   */
  SGNode *clone() {
    // Create a copy of the current node without its children.
    ParentSGNode *newtransform = copyNode();
    // Clone each child and add it to the new node.
    for (int i = 0; i < children.size(); i++) {
      newtransform->addChild(children[i]->clone());
    }
    return newtransform;
  }

  /**
   * @brief Sets the animation transformation matrix.
   *
   * @param m The new animation transformation matrix.
   */
  void setAnimTransform(const glm::mat4 &m) { animTransform = m; }

  /**
   * @brief Retrieves the current animation transformation matrix.
   *
   * @return The current animation transformation matrix.
   */
  glm::mat4 getAnimTransform() const { return animTransform; }

protected:
  vector<SGNode *> children; ///< Container for child nodes.
  glm::mat4 animTransform;   ///< Animation transformation matrix for this node.

  /**
   * @brief Pure virtual function to create a copy of the current node.
   *
   * Derived classes must implement this function to return a new instance that
   * is a copy of the current node (excluding its children).
   *
   * @return Pointer to the copied ParentSGNode.
   */
  virtual ParentSGNode *copyNode() = 0;
};
} // namespace sgraph

#endif
