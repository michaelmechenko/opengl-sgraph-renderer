#ifndef _OBJIMPORTER_H_
#define _OBJIMPORTER_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <vector>
using namespace std;

namespace util
{



/*
 * A helper class to import a PolygonMesh object from an OBJ file.
 * It imports only position, normal and texture coordinate data (if present)
 */
template <class K>
class ObjImporter
{
public:
    static PolygonMesh<K> importFile(ifstream& in, bool scaleAndCenter)
    {
        vector<glm::vec4> vertices,normals,texcoords;
        vector<unsigned int> triangles, triangle_texture_indices, triangle_normal_indices;
        int i,j;
        int lineno;
        PolygonMesh<K> mesh;


        lineno = 0;

        string line;



        while (getline(in,line))
        {
            lineno++;
            if ((line.length()<=0) || (line[0] == '#'))
            {
                //line is a comment, ignore
                continue;
            }

            stringstream str;

            str << line;

            vector<string> tokens;
            string symbol;

            while (str>>symbol)
                tokens.push_back(symbol);

            if (tokens[0]=="v")
            {
                if ((tokens.size()<4) || (tokens.size()>7))
                {
                    str.str("");
                    str.clear();
                    str << "Line " << lineno << ": Vertex coordinate has an invalid number of values";
                    throw str.str();
                }

                float num;
                glm::vec4 v;

                str.str("");
                str.clear();

                str << tokens[1] << " " << tokens[2] << " " << tokens[3];

                str >> v.x >> v.y >> v.z;
                v.w = 1.0f;

                if (tokens.size()==5)
                {
                    str.str("");
                    str.clear();
                    str << tokens[4];
                    str >> num;
                    if (num!=0)
                    {
                        v.x/=num;
                        v.y/=num;
                        v.z/=num;
                    }
                }

                vertices.push_back(v);
            }
            else if (tokens[0]=="vt")
            {
                if ((tokens.size()<3) || (tokens.size()>4))
                {
                    str.str("");
                    str.clear();
                    str << "Line " << lineno << ": Texture coordinate has an invalid number of values";
                    throw str.str();
                }

                glm::vec4 v;

                float n;

                str.str("");
                str.clear();
                str << tokens[1] << " " << tokens[2];

                str >> v.x >> v.y;
                v.z = 0.0f;
                v.w = 1.0f;


                if (tokens.size()>3)
                {
                    str.str("");
                    str.clear();
                    str << tokens[3];
                    str >> v.z;
                }

                texcoords.push_back(v);
            }
            else if (tokens[0]=="vn")
            {
                if (tokens.size()!=4)
                {
                    str.str("");
                    str.clear();
                    str << "Line " << lineno << ": Normal has an invalid number of values";
                    throw str.str();
                }


                float num;
                glm::vec3 v;

                str.str("");
                str.clear();
                str << tokens[1] << " " << tokens[2] << " " << tokens[3];
                str >> v.x >> v.y >> v.z;

                v = glm::normalize(v);
                normals.push_back(glm::vec4(v,0.0f));
            }
            else if (tokens[0]=="f")
            {
                if (tokens.size()<4)
                {
                    str.str("");
                    str.clear();
                    str << "Line " << lineno << ": Face has too few vertices, must be at least 3";
                }


                vector <unsigned int> t_triangles,t_tex,t_normal;


                for (i=1;i<tokens.size();i++)
                {
                    str.str("");
                    str.clear();
                    str << tokens[i];

                    vector<string> data;
                    string temp;

                    while (getline(str,temp,'/'))
                        data.push_back(temp);


                    if ((data.size() < 1) && (data.size() > 3))
                    {
                        str.str("");
                        str.clear();
                        str << "Line " << lineno << ": Face specification has an incorrect number of values";
                        throw str.str();
                    }


                    //in OBJ file format all indices begin at 1, so must subtract 1 here
                    int vi;
                    str.str("");
                    str.clear();
                    str << data[0];
                    str >> vi;
                    t_triangles.push_back(vi-1); //vertex index
                    if (data.size() > 1) {
                        if (data[1].length() > 0) //a vertex texture index exists
                        {
                            str.str("");
                            str.clear();
                            str << data[1];
                            str >> vi;
                            t_tex.push_back(vi-1);
                        }


                        if (data.size() > 2) //a vertex normal index exists
                        {
                            str.str("");
                            str.clear();
                            str << data[2];
                            str >> vi;
                            t_normal.push_back(vi-1);
                        }

                    }
                }

                if (t_triangles.size()<3)
                {
                    str.str("");
                    str.clear();
                    str << "Line " << lineno << ": Fewer than 3 vertices for a polygon";
                    throw str.str();
                }

                //if face has more than 3 vertices, break down into a triangle fan
                for (i=2;i<t_triangles.size();i++)
                {
                    triangles.push_back(t_triangles[0]);
                    triangles.push_back(t_triangles[i-1]);
                    triangles.push_back(t_triangles[i]);

                    if (t_tex.size()>0)
                    {
                        triangle_texture_indices.push_back(t_tex[0]);
                        triangle_texture_indices.push_back(t_tex[i-1]);
                        triangle_texture_indices.push_back(t_tex[i]);
                    }

                    if (t_normal.size()>0)
                    {
                        triangle_normal_indices.push_back(t_normal[0]);
                        triangle_normal_indices.push_back(t_normal[i-1]);
                        triangle_normal_indices.push_back(t_normal[i]);
                    }

                }


            }
        }

        if (scaleAndCenter)
        {
            //center about the origin and within a cube of side 1 centered at the origin
            //find the centroid
            glm::vec4 center = vertices[0];

            glm::vec4 minimum = center;
            glm::vec4 maximum = center;

            for (i=1;i<vertices.size();i++)
            {
                //center = center.add(vertices.get(i).x,vertices.get(i).y,vertices.get(i).z,0.0f);
                minimum = glm::min(minimum,vertices[i]);
                maximum = glm::max(maximum,vertices[i]);
            }

            center = (minimum + maximum)*0.5f;


            float longest;


            longest = std::max(maximum.x-minimum.x,std::max(maximum.y-minimum.y,maximum.z-minimum.z));

            //first translate and then scale
            glm::mat4 transformMatrix = glm::scale(glm::mat4(1.0),
                                                    glm::vec3(1.0f/longest,
                                                              1.0f/longest,
                                                              1.0f/longest))
                                                  * glm::translate(glm::mat4(1.0),
                                                        glm::vec3(-center.x,
                                                                 -center.y,
                                                                 -center.z));

            //scale down each other
            for (i=0;i<vertices.size();i++)
            {
                vertices[i] = transformMatrix * vertices[i];
            }
        }

        vector<K> vertexData;
        vector<float> data;
        for (i=0;i<vertices.size();i++) {
            K v;
			
			data.clear();
			data.push_back(vertices[i].x);
			data.push_back(vertices[i].y);
			data.push_back(vertices[i].z);
			data.push_back(vertices[i].w);
			
            v.setData("position",data);
            if (texcoords.size()==vertices.size())
            {
				data.clear();
				data.push_back(texcoords[i].x);
				data.push_back(texcoords[i].y);
				data.push_back(texcoords[i].z);
				data.push_back(texcoords[i].w);
				v.setData("texcoord",data);
			}    
            if (normals.size()==vertices.size())
			{
				data.clear();
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
				data.push_back(normals[i].w);
				v.setData("normal",data);
			}

            vertexData.push_back(v);
        }

        if ((normals.size()==0) || (normals.size()!=vertices.size()))
            mesh.computeNormals();

        mesh.setVertexData(vertexData);
        mesh.setPrimitives(triangles);
        mesh.setPrimitiveType(GL_TRIANGLES);
        mesh.setPrimitiveSize(3);
        return mesh;
    }
};
}

#endif
