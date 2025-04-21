#include "Model.h"
#include <GLFW/glfw3.h>

/**
 * @brief Constructs a new Model object.
 */
Model::Model() {}

/**
 * @brief Destroys the Model object.
 *
 * Deletes the associated scenegraph if it exists.
 */
Model::~Model() {
  if (scenegraph) {
    delete scenegraph;
  }
}

/**
 * @brief Gets the current scenegraph.
 *
 * @return Pointer to the current scenegraph.
 */
sgraph::IScenegraph *Model::getScenegraph() { return this->scenegraph; }

/**
 * @brief Sets the scenegraph.
 *
 * @param scenegraph Pointer to the new scenegraph.
 */
void Model::setScenegraph(sgraph::IScenegraph *scenegraph) {
  this->scenegraph = scenegraph;
}

/**
 * @brief Sets the texture paths.
 *
 * Updates the texture path mapping with the provided map.
 *
 * @param tp Map containing texture paths keyed by name.
 */
void Model::setTexturePaths(const map<string, string> &tp) {
  texturePaths = tp;
}

/**
 * @brief Retrieves the texture paths.
 *
 * @return A constant reference to the map of texture paths.
 */
const map<string, string> &Model::getTexturePaths() const {
  return texturePaths;
}
