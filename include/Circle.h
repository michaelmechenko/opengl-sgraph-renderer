#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include "VertexAttrib.h"
#include <PolygonMesh.h>
#include <GLFW/glfw3.h>

class Circle: public util::PolygonMesh<VertexAttrib> {
    public:
    Circle(int cx,int cy,int radius);
    ~Circle(){}

};

Circle::Circle(int cx,int cy,int radius) {
    //create the vertex data
    vector<glm::vec4> positions;

    //create a circle using slices
    int SLICES = 100;
    float PI = 3.14159;
    //start with the center
    positions.push_back(glm::vec4(cx,cy,0.0,1.0));
    for (int i=0;i<SLICES;i=i+1) {
        float theta = (float)i/(SLICES-1)* 2*PI;
        float x = cx + radius * cos(theta);
        float y = cy + radius * sin(theta);

        positions.push_back(glm::vec4(x,y,0,1));
    }

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

    //create the indices
    vector<unsigned int> indices;
    for (int i=0;i<positions.size();i=i+1) {
        indices.push_back(i);
    }
    indices.push_back(1);


    this->setVertexData(vertexData);
    // give it the index data that forms the polygons
    this->setPrimitives(indices);

    this->setPrimitiveType(
        GL_TRIANGLE_FAN);         // when rendering specify this to OpenGL
    this->setPrimitiveSize(3);  // 3 vertices per polygon



}

#endif