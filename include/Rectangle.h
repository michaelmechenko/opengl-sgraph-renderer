#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include "VertexAttrib.h"
#include <PolygonMesh.h>
#include <GLFW/glfw3.h>

class Rectangle: public util::PolygonMesh<VertexAttrib> {
    public:
    Rectangle(int x,int y,int width,int height);
    ~Rectangle(){}
};

Rectangle::Rectangle(int x,int y,int width,int height) {
    //create the vertex data
    vector<glm::vec4> positions;
    positions.push_back(glm::vec4(x,y,0,1));
    
    positions.push_back(glm::vec4(x+width,y,0,1));
    
    positions.push_back(glm::vec4(x+width,y+height,0,1));

    positions.push_back(glm::vec4(x,y+height,0,1));

    vector<VertexAttrib> vertexData;
    for (int i=0;i<positions.size();i++) {
        vector<float> data;
        VertexAttrib v;
        for (int j=0;j<4;j++) {
            data.push_back(positions[i][j]);
        }
        v.setData("position",data);
        vertexData.push_back(v);
    }

    vector<unsigned int> indices;
    //triangle 1
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    //triangle 2
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    this->setVertexData(vertexData);
    // give it the index data that forms the polygons
    this->setPrimitives(indices);

    /*
    It turns out, there are several ways of
    reading the list of indices and interpreting
    them as triangles.

    The first, simplest (and the one we have
    assumed above) is to just read the list of
    indices 3 at a time, and use them as triangles.
    In OpenGL, this is the GL_TRIANGLES mode.

    If we wanted to draw lines by reading the indices
    two at a time, we would specify GL_LINES (try this).

    In any case, this "mode" and the actual list of
    indices are related. That is, decide which mode
    you want to use, and accordingly build the list
    of indices.
     */

    this->setPrimitiveType(
        GL_TRIANGLES);         // when rendering specify this to OpenGL
    this->setPrimitiveSize(3);  // 3 vertices per polygon

}

#endif