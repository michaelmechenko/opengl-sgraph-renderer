#ifndef _POLYGONMESH_H_
#define _POLYGONMESH_H_

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <vector>
using namespace std;

namespace util {

/*
 * This class represents a polygon mesh. This class works with any
 * representation of vertex attributes that implements the
 * @link{IVertexData} interface.
 *
 * It stores a polygon mesh as follows:
 *
 * <ul>
 *     <li>A list of vertices: Each vertex is represented by an object that
 *         stores various attributes of that vertex like position, normal,
 *         texture coordinates and others.</li>
 *     <li>A list of indices: these are indices into the above array.
 *         This is called indexed representation and allows us to share vertices
 *         between polygons efficiently.</li>
 *     <li>Data about how to interpret the indices (e.g. read 3 at a time to
 *         form a triangle, read to make a triangle fan, etc.)</li>
 *     <li>How many indices make a polygon (2 for line, 3 for triangle,
 *         4 for quad, etc.)</li>
 * </ul>
 */

template <class VertexType> class PolygonMesh {

public:
  PolygonMesh();
  ~PolygonMesh();
  /*
   * Set the primitive type. The primitive type is represented by an integer.
   * For example in OpenGL, these would be GL_TRIANGLES, GL_TRIANGLE_FAN,
   * GL_QUADS, etc.
   * \param v
   */
  void setPrimitiveType(int v);
  int getPrimitiveType() const;
  /*
   * Sets how many indices make up a primitive.
   * \param s
   */
  void setPrimitiveSize(int s);
  int getPrimitiveSize() const;
  int getPrimitiveCount() const;
  int getVertexCount() const;

  glm::vec4 getMinimumBounds() const;
  glm::vec4 getMaximumBounds() const;
  vector<VertexType> getVertexAttributes() const;
  vector<unsigned int> getPrimitives() const;
  void setVertexData(const vector<VertexType> &vp);
  void setPrimitives(const vector<unsigned int> &t);
  /*
   * Compute vertex normals in this polygon mesh using Newell's method, if
   * position data exists
   */
  void computeNormals();
  /*
   * Compute the bounding box of this polygon mesh, if there is position data
   */
  void computeBoundingBox();

protected:
  vector<VertexType> vertexData;
  vector<unsigned int> primitives;
  int primitiveType;
  int primitiveSize;
  glm::vec4 minBounds, maxBounds; // bounding box
};

template <class VertexType> PolygonMesh<VertexType>::PolygonMesh() {}

template <class VertexType> PolygonMesh<VertexType>::~PolygonMesh() {}

template <class VertexType>
void PolygonMesh<VertexType>::setPrimitiveType(int v) {
  primitiveType = v;
}

template <class VertexType>
int PolygonMesh<VertexType>::getPrimitiveType() const {
  return primitiveType;
}

template <class VertexType>
void PolygonMesh<VertexType>::setPrimitiveSize(int s) {
  primitiveSize = s;
}

template <class VertexType>
int PolygonMesh<VertexType>::getPrimitiveSize() const {
  return primitiveSize;
}

template <class VertexType>
int PolygonMesh<VertexType>::getPrimitiveCount() const {
  return primitives.size();
}

template <class VertexType>
int PolygonMesh<VertexType>::getVertexCount() const {
  return vertexData.size();
}

template <class VertexType>
glm::vec4 PolygonMesh<VertexType>::getMinimumBounds() const {
  return glm::vec4(minBounds);
}

template <class VertexType>
glm::vec4 PolygonMesh<VertexType>::getMaximumBounds() const {
  return glm::vec4(maxBounds);
}

template <class VertexType>
vector<VertexType> PolygonMesh<VertexType>::getVertexAttributes() const {
  return vector<VertexType>(vertexData);
}

template <class VertexType>
vector<unsigned int> PolygonMesh<VertexType>::getPrimitives() const {
  return vector<unsigned int>(primitives);
}

template <class VertexType>
void PolygonMesh<VertexType>::setVertexData(const vector<VertexType> &vp) {
  vertexData = vector<VertexType>(vp);
  computeBoundingBox();
}

template <class VertexType>
void PolygonMesh<VertexType>::setPrimitives(const vector<unsigned int> &t) {
  primitives = vector<unsigned int>(t);
}

template <class VertexType> void PolygonMesh<VertexType>::computeBoundingBox() {
  unsigned int i, j;

  if (vertexData.size() <= 0)
    return;

  if (!vertexData[0].hasData("position")) {
    return;
  }

  vector<glm::vec4> positions;

  for (i = 0; i < vertexData.size(); i++) {
    vector<float> data = vertexData[i].getData("position");
    glm::vec4 pos = glm::vec4(0, 0, 0, 1);
    switch (data.size()) {
    case 4:
      pos.w = data[3];
    case 3:
      pos.z = data[2];
    case 2:
      pos.y = data[1];
    case 1:
      pos.x = data[0];
    }
    positions.push_back(pos);
  }

  minBounds = glm::vec4(positions[0]);
  maxBounds = glm::vec4(positions[0]);

  for (j = 0; j < positions.size(); j++) {
    glm::vec4 p = positions[j];

    if (p.x < minBounds.x) {
      minBounds.x = p.x;
    }

    if (p.x > maxBounds.x) {
      maxBounds.x = p.x;
    }

    if (p.y < minBounds.y) {
      minBounds.y = p.y;
    }

    if (p.y > maxBounds.y) {
      maxBounds.y = p.y;
    }

    if (p.z < minBounds.z) {
      minBounds.z = p.z;
    }

    if (p.z > maxBounds.z) {
      maxBounds.z = p.z;
    }
  }
}

/*
 * Compute vertex normals in this polygon mesh using Newell's method, if
 * position data exists
 */

template <class VertexType> void PolygonMesh<VertexType>::computeNormals() {
  if (vertexData.empty())
    return;
  if (!vertexData[0].hasData("position"))
    return;
  if (!vertexData[0].hasData("normal"))
    return;

  // Build positions vector from vertexData
  vector<glm::vec4> positions;
  positions.reserve(vertexData.size());
  for (size_t i = 0; i < vertexData.size(); i++) {
    vector<float> data = vertexData[i].getData("position");
    glm::vec4 pos(0.0f, 0.0f, 0.0f, 1.0f);
    if (data.size() >= 1)
      pos.x = data[0];
    if (data.size() >= 2)
      pos.y = data[1];
    if (data.size() >= 3)
      pos.z = data[2];
    if (data.size() >= 4)
      pos.w = data[3];
    positions.push_back(pos);
  }

  // Initialize normals vector to zeros
  vector<glm::vec4> normals(positions.size(), glm::vec4(0.0f));

  // Loop over each primitive using Newell's method
  for (size_t i = 0; i < primitives.size(); i += primitiveSize) {
    glm::vec4 norm(0.0f);
    // Gather vertex indices for the primitive
    vector<unsigned int> indices;
    for (size_t k = 0; k < primitiveSize; k++) {
      indices.push_back(primitives[i + k]);
    }
    // Compute the normal for this primitive
    for (size_t k = 0; k < primitiveSize; k++) {
      unsigned int current = indices[k];
      unsigned int next = indices[(k + 1) % primitiveSize];
      norm.x += (positions[current].y - positions[next].y) *
                (positions[current].z + positions[next].z);
      norm.y += (positions[current].z - positions[next].z) *
                (positions[current].x + positions[next].x);
      norm.z += (positions[current].x - positions[next].x) *
                (positions[current].y + positions[next].y);
    }
    norm = glm::normalize(norm);
    // Accumulate the computed normal for each vertex in this primitive
    for (size_t k = 0; k < primitiveSize; k++) {
      normals[indices[k]] += norm;
    }
  }

  // Normalize all accumulated normals
  for (size_t i = 0; i < normals.size(); i++) {
    normals[i] = glm::normalize(normals[i]);
  }

  // Update each vertex's normal attribute
  for (size_t i = 0; i < vertexData.size(); i++) {
    vector<float> n;
    n.push_back(normals[i].x);
    n.push_back(normals[i].y);
    n.push_back(normals[i].z);
    // Setting the w-component to 0 for a direction vector
    n.push_back(0.0f);
    vertexData[i].setData("normal", n);
  }
}

} // namespace util
#endif
