#ifndef __MODEL_H__
#define __MODEL_H__

#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "sgraph/IScenegraph.h"
#include <map>

using namespace std;

class Model {
public:
  Model();
  ~Model();
  sgraph::IScenegraph *getScenegraph();
  void setScenegraph(sgraph::IScenegraph *scenegraph);

  // textures
  void setTexturePaths(const map<string, string> &tp);
  const map<string, string> &getTexturePaths() const;

private:
  sgraph::IScenegraph *scenegraph;
  map<string, string> texturePaths;
};
#endif
