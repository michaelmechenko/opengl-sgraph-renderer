/**
 * @file Model.h
 * @brief Definition of the Model class.
 *
 * This file contains the definition of the Model class which maintains a
 * scenegraph and a texture mapping for a polygon mesh model.
 */

#ifndef __MODEL_H__
#define __MODEL_H__

#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "sgraph/IScenegraph.h"
#include <map>
#include <string>

using namespace std;

/**
 * @class Model
 * @brief Represents a 3D model with an associated scenegraph and texture paths.
 */
class Model {
public:
  /**
   * @brief Constructs a new Model object.
   */
  Model();

  /**
   * @brief Destroys the Model object.
   */
  ~Model();

  /**
   * @brief Retrieves the associated scenegraph.
   *
   * @return sgraph::IScenegraph* Pointer to the current scenegraph.
   */
  sgraph::IScenegraph *getScenegraph();

  /**
   * @brief Sets the scenegraph for the model.
   *
   * @param scenegraph Pointer to the scenegraph to associate.
   */
  void setScenegraph(sgraph::IScenegraph *scenegraph);

  // textures

  /**
   * @brief Sets the texture paths for the model.
   *
   * This function assigns texture file paths mapped by names.
   *
   * @param tp A constant reference to a map where keys are texture names and
   * values are texture file paths.
   */
  void setTexturePaths(const map<string, string> &tp);

  /**
   * @brief Retrieves the texture paths of the model.
   *
   * @return const map<string, string>& A constant reference to the map of
   * texture paths.
   */
  const map<string, string> &getTexturePaths() const;

private:
  /**
   * @brief Pointer to the scenegraph associated with the model.
   */
  sgraph::IScenegraph *scenegraph;

  /**
   * @brief A mapping between texture names and file paths.
   */
  map<string, string> texturePaths;
};

#endif // __MODEL_H__
