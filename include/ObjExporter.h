#ifndef _OBJEXPORTER_H_
#define _OBJEXPORTER_H_

#include <fstream>
#include <vector>
using namespace std;
#include "PolygonMesh.h"
#include <glm/glm.hpp>

namespace util
{
	

	/**
	 * A helper class to export a PolygonMesh object to file using the OBJ file format
	 * This exporter only writes the position, normal and texture coordinate data. It
	 * ignores any other attributes
	 */
	template <class K>
	class ObjExporter
	{
		public:
			static bool exportFile(const PolygonMesh<K>& mesh,ofstream& out) throw (runtime_error)
			{
				int i,j;

                vector<K> vertexData = mesh.getVertexAttributes();
				if (vertexData.size()==0)
					return true;

                vector<glm::vec4> vertices,normals,texcoords;
                vector<unsigned int> primitives = mesh.getPrimitives();

				for (i=0;i<vertexData.size();i++) {
					if (vertexData[i].hasData("position")) {
						vector<float> data = vertexData[i].getData("position");
						out << "v ";
						for (j=0;j<data.size();j++) {
							out << data[j] << " ";
						}
						out << endl;
					}

				}

				for (int i=0;i<vertexData.size();i++) {
					if (vertexData[i].hasData("normal")) {
						vector<float> data = vertexData[i].getData("normal");
                        if (data.size()<3) 
                        {
                            throw runtime_error("Normal data must have 3 or 4 numbers, with the 4th number being 0");
                        }
						out << "vn ";
						for (j=0;j<3;j++) {
							out << data[j] << " ";
						}
						out << endl;
					}

				}

				for (int i=0;i<vertexData.size();i++) {
					if (vertexData[i].hasData("texcoord")) {
						vector<float> data = vertexData[i].getData("texcoord");
                        if (data.size()<3) 
                        {
                            throw runtime_error("Texture coordinate data must have 3 or 4 numbers, with the 4th number being 1");
                        }
						out << "vt ";
						for (j=0;j<3;j++) {
							out << data[j] << " ";
						}
						out << endl;
					}

				}


				//polygons

				for (i=0;i<primitives.size();i+=mesh.getPrimitiveSize())
				{
					out << "f ";
					for (j=0;j<mesh.getPrimitiveSize();j++)
					{
						//in OBJ file format indices begin at 1, so we must add 1 here
                        out << primitives[i+j]+1 << " ";
					}
					out << endl;
				}
				return true;
			}
	};
}

#endif
