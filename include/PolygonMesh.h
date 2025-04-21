#ifndef _POLYGONMESH_H_
#define _POLYGONMESH_H_

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <vector>
using namespace std;

namespace util
{


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

template <class VertexType>
class PolygonMesh
{


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
    void setVertexData(const vector<VertexType>& vp);
    void setPrimitives(const vector<unsigned int>& t);
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
    glm::vec4 minBounds,maxBounds; //bounding box

};

template<class VertexType>
PolygonMesh<VertexType>::PolygonMesh()
{

}

template<class VertexType>
PolygonMesh<VertexType>::~PolygonMesh()
{

}


template<class VertexType>
void PolygonMesh<VertexType>::setPrimitiveType(int v)
{
    primitiveType = v;
}

template<class VertexType>
int PolygonMesh<VertexType>::getPrimitiveType() const { return primitiveType;}

template<class VertexType>
void PolygonMesh<VertexType>::setPrimitiveSize(int s)
{
    primitiveSize = s;
}

template<class VertexType>
int PolygonMesh<VertexType>::getPrimitiveSize() const
{
    return primitiveSize;
}

template<class VertexType>
int PolygonMesh<VertexType>::getPrimitiveCount() const
{
    return primitives.size();
}

template<class VertexType>
int PolygonMesh<VertexType>::getVertexCount() const
{
    return vertexData.size();
}

template<class VertexType>
glm::vec4 PolygonMesh<VertexType>::getMinimumBounds() const
{
    return glm::vec4(minBounds);
}

template<class VertexType>
glm::vec4 PolygonMesh<VertexType>::getMaximumBounds() const
{
    return glm::vec4(maxBounds);
}



template<class VertexType>
vector<VertexType> PolygonMesh<VertexType>::getVertexAttributes() const
{
    return vector<VertexType>(vertexData);
}

template<class VertexType>
vector<unsigned int> PolygonMesh<VertexType>::getPrimitives() const
{
    return vector<unsigned int>(primitives);
}

template <class VertexType>
void PolygonMesh<VertexType>::setVertexData(const vector<VertexType>& vp)
{
    vertexData = vector<VertexType>(vp);
    computeBoundingBox();
}

template<class VertexType>
void PolygonMesh<VertexType>::setPrimitives(const vector<unsigned int>& t)
{
    primitives = vector<unsigned int>(t);
}


template<class VertexType>
void PolygonMesh<VertexType>::computeBoundingBox()
{
    unsigned int i,j;

    if (vertexData.size()<=0)
        return;

    if (!vertexData[0].hasData("position"))
    {
        return;
    }

    vector<glm::vec4> positions;

    for (i=0;i<vertexData.size();i++)
    {
        vector<float> data = vertexData[i].getData("position");
        glm::vec4 pos = glm::vec4(0,0,0,1);
        switch (data.size()) {
        case 4: pos.w = data[3];
        case 3: pos.z = data[2];
        case 2: pos.y = data[1];
        case 1: pos.x = data[0];
        }
        positions.push_back(pos);
    }

    minBounds = glm::vec4(positions[0]);
    maxBounds = glm::vec4(positions[0]);

    for (j=0;j<positions.size();j++)
    {
        glm::vec4 p = positions[j];

        if (p.x<minBounds.x)
        {
            minBounds.x = p.x;
        }

        if (p.x>maxBounds.x)
        {
            maxBounds.x = p.x;
        }

        if (p.y<minBounds.y)
        {
            minBounds.y = p.y;
        }

        if (p.y>maxBounds.y)
        {
            maxBounds.y = p.y;
        }

        if (p.z<minBounds.z)
        {
            minBounds.z = p.z;
        }

        if (p.z>maxBounds.z)
        {
            maxBounds.z = p.z;
        }
    }
}

/*
 * Compute vertex normals in this polygon mesh using Newell's method, if
 * position data exists
 */

template<class VertexType>
void PolygonMesh<VertexType>::computeNormals()
{
    int i,j,k;

    if (vertexData.size()<=0)
        return;

    if (!vertexData[0].hasData("position"))
    {
        return;
    }

    if (!vertexData[0].hasData("normal"))
        return;

    vector<glm::vec4> positions;

    for (i=0;i<positions.size();i++) {
        vector<float> data = vertexData[i].getData("position");
        glm::vec4 pos;
        switch (data.size()) {
        case 4: pos.w = data[3];
        case 3: pos.z = data[2];
        case 2: pos.y = data[1];
        case 1: pos.x = data[0];
        }
        positions.push_back(pos);
    }
    vector<glm::vec4> normals;

    for (i=0;i<positions.size();i++)
    {
        normals.push_back(glm::vec4(0.0f,0.0f,0.0f,0.0f));
    }

    for (i=0;i<primitives.size();i+=primitiveSize)
    {
        glm::vec4 norm = glm::vec4(0.0f,0.0f,0.0f,0.0f);



        //compute the normal of this triangle
        vector<unsigned int>v;

        for (k=0;k<primitiveSize;k++)
        {
            v.push_back(primitives[i+k]);
        }

        //the newell's method to calculate normal

        for (k=0;k<primitiveSize;k++)
        {
            norm.x += (positions[v[k]].y-positions[v[(k+1)%primitiveSize]].y)*
                      (positions[v[k]].z+positions[v[(k+1)%primitiveSize]].z);
            norm.y += (positions[v[k]].z-positions[v[(k+1)%primitiveSize]].z)*
                      (positions[v[k]].x+positions[v[(k+1)%primitiveSize]].x);
            norm.z += (positions[v[k]].x-positions[v[(k+1)%primitiveSize]].x)*
                      (positions[v[k]].y+positions[v[(k+1)%primitiveSize]].y);
        }
        norm = glm::normalize(norm);

        for (k=0;k<primitiveSize;k++)
        {
            normals[v[k]] = normals[v[k]] + norm;
        }
    }

    for (i=0;i<normals.size();i++)
    {

        normals[i] = glm::normalize(normals[i]);
    }
    for (i=0;i<vertexData.size();i++) {
        vector<float> n;

        n.push_back(normals[i].x);
        n.push_back(normals[i].y);
        n.push_back(normals[i].z);
        n.push_back(normals[i].w);
        vertexData[i].setData("normal",n);
    }
}


}
#endif
