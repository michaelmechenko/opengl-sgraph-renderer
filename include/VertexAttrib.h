#ifndef _VERTEXATTRIB_H_
#define _VERTEXATTRIB_H_

#include <glm/glm.hpp>
#include "IVertexData.h"
#include <sstream>



/*
 * This class represents the attributes of a single vertex. It is useful in building
 * PolygonMesh objects for many examples.
 *
 * It implements the IVertexData interface so that it can be converted into an array
 * of floats, to work with OpenGL buffers
 */
class VertexAttrib:public util::IVertexData
{


public:
    VertexAttrib()
    {
        position = glm::vec4(0,0,0,1);

    }

    ~VertexAttrib(){}



    bool hasData(string attribName)
    {
        if (attribName == "position")
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    vector<float> getData(string attribName)
    {
        vector<float> result;
        stringstream message;


        if (attribName == "position")
        {
            result.push_back(position.x);
            result.push_back(position.y);
            result.push_back(position.z);
            result.push_back(position.w);
        }
        else
        {
            message << "No attribute: " << attribName << " found!";
            throw runtime_error(message.str());
        }

        return result;
    }

    void setData(string attribName, const vector<float>& data) 
    {
        stringstream message;

        if (attribName == "position")
        {
            position = glm::vec4(0,0,0,1);
            switch (data.size()) {
            case 4: position.w = data[3];
            case 3: position.z = data[2];
            case 2: position.y = data[1];
            case 1: position.x = data[0];
                break;
            default:
                message << "Too much data for attribute: " << attribName;
                throw runtime_error(message.str());
            }
        }       
        else
        {
            message << "Attribute: " << attribName << " unsupported!";
            throw runtime_error(message.str());
        }
    }

    vector<string> getAllAttributes()
    {
        vector<string> attributes;

        attributes.push_back("position");
        return attributes;
    }

private:
    glm::vec4 position;
};

#endif
