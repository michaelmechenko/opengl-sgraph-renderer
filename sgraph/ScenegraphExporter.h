#ifndef _SCENEGRAPHEXPORTER_H_
#define _SCENEGRAPHEXPORTER_H_

#include "GroupNode.h"
#include "LeafNode.h"
#include "RotateTransform.h"
#include "SGNodeVisitor.h"
#include "ScaleTransform.h"
#include "TransformNode.h"
#include "TranslateTransform.h"
#include <sstream>
using namespace std;

namespace sgraph {
class ScenegraphExporter : public SGNodeVisitor {
public:
  ScenegraphExporter(map<string, string> &meshPaths) {
    level = 1;
    number = 0;

    for (map<string, string>::iterator it = meshPaths.begin();
         it != meshPaths.end(); it++) {
      append("instance " + it->first + " " + it->second);
    }
  }

  string getOutput() { return output.str(); }

  /**
   * @brief Recur to the children for drawing
   *
   * @param groupNode
   */
  void visitGroupNode(GroupNode *groupNode) {
    string varname;
    stringstream namestream;
    namestream << "node-" << level << "-" << number;
    varname = namestream.str();

    append("group " + varname + " " + groupNode->getName());

    visitParentSGNode(groupNode, varname);

    if (level == 1) {
      append("assign-root " + varname);
    }
  }

  /**
   * @brief Draw the instance for the leaf, after passing the
   * modelview and color to the shader
   *
   * @param leafNode
   */
  void visitLeafNode(LeafNode *leafNode) {
    string varname;
    stringstream namestream;
    namestream << "node-" << level << "-" << number;
    varname = namestream.str();

    append("leaf " + varname + " " + leafNode->getName() + " " + "instanceof " +
           leafNode->getInstanceOf());

    append("material mat-" + varname);
    stringstream mat;
    mat << "emission " << leafNode->getMaterial().getEmission()[0] << " "
        << leafNode->getMaterial().getEmission()[1] << " "
        << leafNode->getMaterial().getEmission()[2] << endl;
    mat << "ambient " << leafNode->getMaterial().getAmbient()[0] << " "
        << leafNode->getMaterial().getAmbient()[1] << " "
        << leafNode->getMaterial().getAmbient()[2] << endl;
    mat << "diffuse " << leafNode->getMaterial().getDiffuse()[0] << " "
        << leafNode->getMaterial().getDiffuse()[1] << " "
        << leafNode->getMaterial().getDiffuse()[2] << endl;
    mat << "specular " << leafNode->getMaterial().getSpecular()[0] << " "
        << leafNode->getMaterial().getSpecular()[1] << " "
        << leafNode->getMaterial().getSpecular()[2] << endl;
    mat << "shininess " << leafNode->getMaterial().getShininess();
    append(mat.str());
    append("end-material");
    append("assign-material " + varname + " mat-" + varname);

    if (level == 1) {
      append("assign-root " + varname);
    }
  }

  /**
   * @brief Multiply the transform to the modelview and recur to child
   *
   * @param transformNode
   */
  void visitTransformNode(TransformNode *transformNode) {}

  /**
   * @brief For this visitor, only the transformation matrix is required.
   * Thus there is nothing special to be done for each type of transformation.
   * We delegate to visitTransformNode above
   *
   * @param scaleNode
   */
  void visitScaleTransform(ScaleTransform *scaleNode) {
    string varname;
    stringstream namestream;
    namestream << "node-" << level << "-" << number;
    varname = namestream.str();

    stringstream t;
    t << "scale " << varname << " " << scaleNode->getName() + " "
      << scaleNode->getScale()[0] << " " << scaleNode->getScale()[1] << " "
      << scaleNode->getScale()[2];

    append(t.str());

    visitParentSGNode(scaleNode, varname);

    if (level == 1) {
      append("assign-root " + varname);
    }
  }

  /**
   * @brief For this visitor, only the transformation matrix is required.
   * Thus there is nothing special to be done for each type of transformation.
   * We delegate to visitTransformNode above
   *
   * @param translateNode
   */
  void visitTranslateTransform(TranslateTransform *translateNode) {
    string varname;
    stringstream namestream;
    namestream << "node-" << level << "-" << number;
    varname = namestream.str();

    stringstream t;
    t << "translate " << varname << " " + translateNode->getName()
      << translateNode->getTranslate()[0] << " "
      << translateNode->getTranslate()[1] << " "
      << translateNode->getTranslate()[2];

    append(t.str());

    visitParentSGNode(translateNode, varname);

    if (level == 1) {
      append("assign-root " + varname);
    }
  }

  void visitRotateTransform(RotateTransform *rotateNode) {
    string varname;
    stringstream namestream;
    namestream << "node-" << level << "-" << number;
    varname = namestream.str();

    stringstream t;
    t << "rotate " << varname << " " + rotateNode->getName()
      << glm::degrees(rotateNode->getAngleInRadians()) << " "
      << rotateNode->getRotationAxis()[0] << " "
      << rotateNode->getRotationAxis()[1] << " "
      << rotateNode->getRotationAxis()[2];

    append(t.str());

    visitParentSGNode(rotateNode, varname);

    if (level == 1) {
      append("assign-root " + varname);
    }
  }

private:
  void append(const string &str) { output << str << endl; }

  void visitParentSGNode(ParentSGNode *node, const string &name) {
    level += 1;
    int old = number;
    number = 0;

    for (int i = 0; i < node->getChildren().size(); i = i + 1) {
      node->getChildren()[i]->accept(this);
      stringstream childname;
      childname << "node-" << level << "-" << number;
      append("add-child " + childname.str() + " " + name);
      number = number + 1;
    }
    level -= 1;
    number = old;
  }
  int level;  // the level of the scene graph
  int number; // the number of the current node at the current level (only
              // changes with parent sg nodes)
  stringstream output;
};
} // namespace sgraph

#endif
