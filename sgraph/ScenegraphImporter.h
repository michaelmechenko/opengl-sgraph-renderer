#ifndef _SCENEGRAPHIMPORTER_H_
#define _SCENEGRAPHIMPORTER_H_

#include "GroupNode.h"
#include "IScenegraph.h"
#include "LeafNode.h"
#include "Light.h"
#include "Material.h"
#include "PolygonMesh.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "Scenegraph.h"
#include "TransformNode.h"
#include "TranslateTransform.h"
#include "VertexAttrib.h"
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <sstream>
#include <string>
using namespace std;
namespace sgraph {

class ScenegraphImporter {
public:
  ScenegraphImporter() {}

  IScenegraph *parse(istream &input) {
    string command;
    string inputWithOutCommentsString = stripComments(input);
    istringstream inputWithOutComments(inputWithOutCommentsString);

    while (inputWithOutComments >> command) {
      if (command == "instance") {
        string name, path;
        inputWithOutComments >> name >> path;
        meshPaths[name] = path;
        ifstream in(path);
        if (in.is_open()) {
          util::PolygonMesh<VertexAttrib> mesh =
              util::ObjImporter<VertexAttrib>::importFile(in, true);
          meshes[name] = mesh;
        }
      } else if (command == "group") {
        parseGroup(inputWithOutComments);
      } else if (command == "leaf") {
        parseLeaf(inputWithOutComments);
      } else if (command == "material") {
        parseMaterial(inputWithOutComments);
      } else if (command == "scale") {
        parseScale(inputWithOutComments);
      } else if (command == "rotate") {
        parseRotate(inputWithOutComments);
      } else if (command == "translate") {
        parseTranslate(inputWithOutComments);
      } else if (command == "copy") {
        parseCopy(inputWithOutComments);
      } else if (command == "import") {
        parseImport(inputWithOutComments);
      } else if (command == "assign-material") {
        parseAssignMaterial(inputWithOutComments);
      } else if (command == "add-child") {
        parseAddChild(inputWithOutComments);
      } else if (command == "assign-root") {
        parseSetRoot(inputWithOutComments);
        // lights
      } else if (command == "light") {
        parseLight(inputWithOutComments);
      } else if (command == "assign-light") {
        parseAssignLight(inputWithOutComments);
      } // image
      else if (command == "image") {
        parseImage(inputWithOutComments);
        // texture
      } else if (command == "assign-texture") {
        parseAssignTexture(inputWithOutComments);
      } else {
        throw runtime_error("Unrecognized or out-of-place command: " + command);
      }
    }
    if (root != NULL) {
      IScenegraph *scenegraph = new Scenegraph();
      scenegraph->makeScenegraph(root);
      scenegraph->setMeshes(meshes);
      scenegraph->setMeshPaths(meshPaths);
      return scenegraph;
    } else {
      throw runtime_error("Parsed scene graph, but nothing set as root");
    }
  }

protected:
  // get texture paths
  const map<string, string> &getTexturePaths() const { return texturePaths; }

  virtual void parseGroup(istream &input) {
    string varname, name;
    input >> varname >> name;
    SGNode *group = new GroupNode(name, NULL);
    nodes[varname] = group;
  }

  virtual void parseLeaf(istream &input) {
    string varname, name, command, instanceof;
    input >> varname >> name;
    input >> command;
    if (command == "instanceof") {
      input >> instanceof;
    }
    SGNode *leaf = new LeafNode(instanceof, name, NULL);
    nodes[varname] = leaf;
  }

  virtual void parseScale(istream &input) {
    string varname, name;
    input >> varname >> name;
    float sx, sy, sz;
    input >> sx >> sy >> sz;
    SGNode *scaleNode = new ScaleTransform(sx, sy, sz, name, NULL);
    nodes[varname] = scaleNode;
  }

  virtual void parseTranslate(istream &input) {
    string varname, name;
    input >> varname >> name;
    float tx, ty, tz;
    input >> tx >> ty >> tz;
    SGNode *translateNode = new TranslateTransform(tx, ty, tz, name, NULL);
    nodes[varname] = translateNode;
  }

  virtual void parseRotate(istream &input) {
    string varname, name;
    input >> varname >> name;
    float angleInDegrees, ax, ay, az;
    input >> angleInDegrees >> ax >> ay >> az;
    SGNode *rotateNode = new RotateTransform(glm::radians(angleInDegrees), ax,
                                             ay, az, name, NULL);
    nodes[varname] = rotateNode;
  }

  virtual void parseImage(istream &input) {
    string texName, texPath;
    input >> texName >> texPath;
    texturePaths[texName] = texPath;
  }

  virtual void parseAssignTexture(istream &input) {
    string nodeName, texName;
    input >> nodeName >> texName;
    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[nodeName]);
    if (leafNode)
      leafNode->setTexture(texName);
  }

  virtual void parseMaterial(istream &input) {
    util::Material mat;
    float r, g, b;
    string name;
    input >> name;
    string command;
    input >> command;
    while (command != "end-material") {
      if (command == "ambient") {
        input >> r >> g >> b;
        mat.setAmbient(r, g, b);
      } else if (command == "diffuse") {
        input >> r >> g >> b;
        mat.setDiffuse(r, g, b);
      } else if (command == "specular") {
        input >> r >> g >> b;
        mat.setSpecular(r, g, b);
      } else if (command == "emission") {
        input >> r >> g >> b;
        mat.setEmission(r, g, b);
      } else if (command == "shininess") {
        input >> r;
        mat.setShininess(r);
        // reflections
      } else if (command == "absorption") {
        input >> r;
        mat.setAbsorption(r);
      } else if (command == "reflection") {
        input >> r;
        mat.setReflection(r);
      } else if (command == "transparency") {
        input >> r;
        mat.setTransparency(r);
      } else if (command == "refractive-index") {
        input >> r;
        mat.setRefractiveIndex(r);
      }
      input >> command;
    }
    materials[name] = mat;
  }

  virtual void parseCopy(istream &input) {
    string nodename, copyof;
    input >> nodename >> copyof;
    if (nodes.find(copyof) != nodes.end()) {
      SGNode *copy = nodes[copyof]->clone();
      nodes[nodename] = copy;
    }
  }

  virtual void parseImport(istream &input) {
    string nodename, filepath;
    input >> nodename >> filepath;
    ifstream external_scenegraph_file(filepath);
    if (external_scenegraph_file.is_open()) {
      IScenegraph *importedSG = parse(external_scenegraph_file);
      nodes[nodename] = importedSG->getRoot();
      // no need to import meshes/paths for now
      importedSG->makeScenegraph(NULL);
      delete importedSG;
    }
  }

  // parse light
  virtual void parseLight(istream &input) {
    string varname;
    input >> varname;
    util::Light l;
    string command;
    while (input >> command && command != "end-light") {
      if (command == "ambient") {
        float r, g, b;
        input >> r >> g >> b;
        l.setAmbient(r, g, b);
      } else if (command == "diffuse") {
        float r, g, b;
        input >> r >> g >> b;
        l.setDiffuse(r, g, b);
      } else if (command == "specular") {
        float r, g, b;
        input >> r >> g >> b;
        l.setSpecular(r, g, b);
      } else if (command == "position") {
        float x, y, z;
        input >> x >> y >> z;
        l.setPosition(x, y, z);
      } else if (command == "spot-direction") {
        float x, y, z;
        input >> x >> y >> z;
        l.setSpotDirection(x, y, z);
      } else if (command == "spot-angle") {
        float angle;
        input >> angle;
        l.setSpotAngle(angle);
      }
    }
    lightTable[varname] = l;
  }

  // parse assign light
  virtual void parseAssignLight(istream &input) {
    string nodeName, lightName;
    input >> nodeName >> lightName;
    if (nodes.find(nodeName) != nodes.end() &&
        lightTable.find(lightName) != lightTable.end()) {
      AbstractSGNode *absNode = dynamic_cast<AbstractSGNode *>(nodes[nodeName]);
      if (absNode)
        absNode->addLight(lightTable[lightName]);
    }
  }

  virtual void parseAssignMaterial(istream &input) {
    string nodename, matname;
    input >> nodename >> matname;
    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[nodename]);
    if ((leafNode != NULL) && (materials.find(matname) != materials.end())) {
      leafNode->setMaterial(materials[matname]);
    }
  }

  virtual void parseAddChild(istream &input) {
    string childname, parentname;
    input >> childname >> parentname;
    ParentSGNode *parentNode = dynamic_cast<ParentSGNode *>(nodes[parentname]);
    SGNode *childNode = NULL;
    if (nodes.find(childname) != nodes.end())
      childNode = nodes[childname];
    if ((parentNode != NULL) && (childNode != NULL)) {
      parentNode->addChild(childNode);
    }
  }

  virtual void parseSetRoot(istream &input) {
    string rootname;
    input >> rootname;
    root = nodes[rootname];
  }

  string stripComments(istream &input) {
    string line, clean;
    while (getline(input, line)) {
      int i = 0;
      while ((i < line.length()) && (line[i] != '#')) {
        clean.push_back(line[i]);
        i++;
      }
      clean.push_back('\n');
    }
    return clean;
  }

private:
  map<string, SGNode *> nodes;
  map<string, util::Material> materials;
  map<string, util::PolygonMesh<VertexAttrib>> meshes;
  map<string, string> meshPaths;
  SGNode *root;
  map<string, string> texturePaths;

  map<string, util::Light> lightTable;
};
} // namespace sgraph
#endif
