#ifndef _RAYCASTSCENEGRAPHRENDERER_H_
#define _RAYCASTSCENEGRAPHRENDERER_H_

#include "GroupNode.h"
#include "LeafNode.h"
#include "Material.h"
#include "Rays.h"
#include "RotateTransform.h"
#include "SGNodeVisitor.h"
#include "ScaleTransform.h"
#include "TransformNode.h"
#include "TranslateTransform.h"
#include <ObjectInstance.h>
#include <cmath>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <limits>
#include <map>
#include <stack>
#include <vector>

namespace sgraph {

class RaycastScenegraphRenderer : public SGNodeVisitor {
public:
  RaycastScenegraphRenderer(std::stack<glm::mat4> &mv,
                            std::map<std::string, util::ObjectInstance *> &os,
                            int width, int height)
      : modelview(mv), objects(os), imageWidth(width), imageHeight(height) {
    imageBuffer.resize(imageWidth * imageHeight, glm::vec3(0.0f));
    backgroundColor = glm::vec3(0.0f);
    viewPlaneZ = -1.0f;
  }

  // render scene graph and write the output to a PPM file
  void render(SGNode *root, const std::string &outputFile) {
    this->root = root;
    glm::mat4 viewTransform = modelview.top();
    glm::mat4 invView = glm::inverse(viewTransform);
    glm::vec3 eye = glm::vec3(invView * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    for (int j = 0; j < imageHeight; j++) {
      for (int i = 0; i < imageWidth; i++) {
        float ndcX = (2.0f * i) / (imageWidth - 1) - 1.0f;
        float ndcY = 1.0f - (2.0f * j) / (imageHeight - 1);
        glm::vec3 pixelPoint(ndcX, ndcY, viewPlaneZ);
        glm::vec3 worldPixel = glm::vec3(invView * glm::vec4(pixelPoint, 1.0f));
        glm::vec3 rayDir = glm::normalize(worldPixel - eye);
        currentRay = Ray(eye, rayDir);

        // reset hit record before casting a new ray
        currentHit.t = std::numeric_limits<float>::max();
        currentHit.material = nullptr;

        modelview.push(glm::mat4(1.0f));
        root->accept(this);
        modelview.pop();

        glm::vec3 pixelColor = backgroundColor;
        if (currentHit.t < std::numeric_limits<float>::max() &&
            currentHit.material)
          pixelColor = shade(currentHit, currentRay, maxBounce);
        imageBuffer[j * imageWidth + i] = pixelColor;
      }
    }
    writePPM(outputFile);
  }

  // for group nodes, simply recur on each child
  virtual void visitGroupNode(GroupNode *groupNode) {
    for (size_t i = 0; i < groupNode->getChildren().size(); i++) {
      groupNode->getChildren()[i]->accept(this);
    }
  }

  // at leaf, test for intersection with the object instance
  virtual void visitLeafNode(LeafNode *leafNode) {
    std::string instanceName = leafNode->getInstanceOf();
    std::shared_ptr<util::Material> material =
        std::make_shared<util::Material>(leafNode->getMaterial());
    glm::mat4 M = modelview.top();
    glm::mat4 invM = glm::inverse(M);
    Ray localRay = transformRay(currentRay, invM);
    HitRecord localHit;
    localHit.t = std::numeric_limits<float>::max();
    bool hit = false;
    if (instanceName.find("box") != std::string::npos) {
      hit = intersectBox(localRay, localHit);
    } else if (instanceName.find("sphere") != std::string::npos) {
      hit = intersectSphere(localRay, localHit);
    }
    if (hit && localHit.t < currentHit.t && localHit.t > 0.0f) {
      glm::vec4 viewPoint = M * glm::vec4(localHit.point, 1.0f);
      currentHit.point = glm::vec3(viewPoint) / viewPoint.w;
      glm::mat4 invTrans = glm::transpose(invM);
      glm::vec4 viewNormal = invTrans * glm::vec4(localHit.normal, 0.0f);
      currentHit.normal = glm::normalize(glm::vec3(viewNormal));
      currentHit.t = localHit.t;
      currentHit.material = material;
    }
  }

  // for transform nodes, push the current modelview, apply the transform, recur
  // and then pop
  virtual void visitTransformNode(TransformNode *transformNode) {
    modelview.push(modelview.top());
    modelview.top() = modelview.top() * transformNode->getTransform();
    for (size_t i = 0; i < transformNode->getChildren().size(); i++) {
      transformNode->getChildren()[i]->accept(this);
    }
    modelview.pop();
  }
  virtual void visitScaleTransform(ScaleTransform *scaleNode) {
    visitTransformNode(scaleNode);
  }
  virtual void visitTranslateTransform(TranslateTransform *translateNode) {
    visitTransformNode(translateNode);
  }
  virtual void visitRotateTransform(RotateTransform *rotateNode) {
    visitTransformNode(rotateNode);
  }

private:
  std::stack<glm::mat4> &modelview;
  std::map<std::string, util::ObjectInstance *> &objects;
  int imageWidth, imageHeight;
  std::vector<glm::vec3> imageBuffer;
  glm::vec3 backgroundColor;
  float viewPlaneZ;
  int maxBounce = 5;
  SGNode *root;
  Ray currentRay = Ray(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
  HitRecord currentHit;

  // transform a ray using given transformation matrix
  Ray transformRay(const Ray &ray, const glm::mat4 &mat) {
    glm::vec4 newOrigin = mat * glm::vec4(ray.origin, 1.0f);
    glm::vec4 newDir = mat * glm::vec4(ray.direction, 0.0f);
    return Ray(glm::vec3(newOrigin) / newOrigin.w,
               glm::normalize(glm::vec3(newDir)));
  }

  // intersection with sphere of radius 1 centered at origin
  bool intersectSphere(const Ray &ray, HitRecord &hit) {
    float radius = 1.0f;
    float A = glm::dot(ray.direction, ray.direction);
    float B = 2.0f * glm::dot(ray.origin, ray.direction);
    float C = glm::dot(ray.origin, ray.origin) - radius * radius;
    float disc = B * B - 4 * A * C;
    if (disc < 0.0f)
      return false;
    float sqrtDisc = std::sqrt(disc);
    float t0 = (-B - sqrtDisc) / (2 * A);
    float t1 = (-B + sqrtDisc) / (2 * A);
    float t = (t0 > 0.0f) ? t0 : t1;
    if (t < 0.0f)
      return false;
    hit.t = t;
    hit.point = ray.origin + t * ray.direction;
    hit.normal = glm::normalize(hit.point);
    return true;
  }

  // intersection with axis-aligned box bounded at [-0.5, 0.5] along each axis
  bool intersectBox(const Ray &ray, HitRecord &hit) {
    glm::vec3 minB(-0.5f), maxB(0.5f);
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();
    for (int i = 0; i < 3; i++) {
      if (std::fabs(ray.direction[i]) < 1e-6f) {
        if (ray.origin[i] < minB[i] || ray.origin[i] > maxB[i])
          return false;
      } else {
        float invD = 1.0f / ray.direction[i];
        float t1 = (minB[i] - ray.origin[i]) * invD;
        float t2 = (maxB[i] - ray.origin[i]) * invD;
        if (t1 > t2)
          std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
        if (tmax < tmin)
          return false;
      }
    }
    hit.t = (tmin >= 0.0f) ? tmin : tmax;
    if (hit.t < 0.0f)
      return false;
    hit.point = ray.origin + hit.t * ray.direction;
    const float bias = 1e-4f;
    if (std::fabs(hit.point.x - minB.x) < bias)
      hit.normal = glm::vec3(-1, 0, 0);
    else if (std::fabs(hit.point.x - maxB.x) < bias)
      hit.normal = glm::vec3(1, 0, 0);
    else if (std::fabs(hit.point.y - minB.y) < bias)
      hit.normal = glm::vec3(0, -1, 0);
    else if (std::fabs(hit.point.y - maxB.y) < bias)
      hit.normal = glm::vec3(0, 1, 0);
    else if (std::fabs(hit.point.z - minB.z) < bias)
      hit.normal = glm::vec3(0, 0, -1);
    else if (std::fabs(hit.point.z - maxB.z) < bias)
      hit.normal = glm::vec3(0, 0, 1);
    else
      hit.normal = glm::vec3(0, 0, 0);
    return true;
  }

  // recursively shade a hit by casting reflection rays
  glm::vec3 shade(const HitRecord &hit, const Ray &ray, int bounce) {
    const float epsilon = 1e-3f;
    glm::vec3 ambient = glm::vec3(hit.material->getAmbient());
    glm::vec3 color = ambient;

    std::vector<glm::vec3> lightPositions = {glm::vec3(0, 0, 30),
                                             glm::vec3(0, 10, 0)};
    std::vector<glm::vec3> lightIntensities = {glm::vec3(0.5f),
                                               glm::vec3(0.5f)};

    for (size_t i = 0; i < lightPositions.size(); i++) {
      glm::vec3 L = glm::normalize(lightPositions[i] - hit.point);
      // offset the shadow ray start slightly to prevent self-intersection
      Ray shadowRay(hit.point + epsilon * hit.normal, L);
      HitRecord shadowHit;
      shadowHit.t = std::numeric_limits<float>::max();

      bool inShadow = ((intersectBox(shadowRay, shadowHit) ||
                        intersectSphere(shadowRay, shadowHit)) &&
                       shadowHit.t > epsilon);

      if (!inShadow) {
        float diff = std::max(glm::dot(hit.normal, L), 0.0f);
        glm::vec3 diffuse = HadamardProduct(
            glm::vec3(hit.material->getDiffuse()) * diff, lightIntensities[i]);
        glm::vec3 R = glm::reflect(-L, hit.normal);
        glm::vec3 V = glm::normalize(ray.origin - hit.point);
        float specAngle = std::max(glm::dot(V, R), 0.0f);
        float spec = std::pow(specAngle, hit.material->getShininess());
        glm::vec3 specular = HadamardProduct(
            glm::vec3(hit.material->getSpecular()) * spec, lightIntensities[i]);
        color += diffuse + specular;
      }
    }

    glm::vec3 reflectionColor(0.0f);
    if (bounce > 0 && hit.material->getReflection() > 0.0f) {
      const float epsilonReflection = 1e-2f; // increased offset for reflection
      glm::vec3 reflectDir = glm::reflect(ray.direction, hit.normal);
      Ray reflectionRay(hit.point + epsilonReflection * hit.normal, reflectDir);
      reflectionColor = traceRay(reflectionRay, bounce - 1);
    }

    return color * hit.material->getAbsorption() +
           reflectionColor * hit.material->getReflection();
  }

  // vector math helper
  inline glm::vec3 HadamardProduct(const glm::vec3 &a, const glm::vec3 &b) {
    return glm::vec3(a.x * b.x, a.y * b.y, a.z * b.z);
  }

  // recursively trace ray to maximum bounce count
  glm::vec3 traceRay(const Ray &ray, int bounce) {
    if (bounce <= 0)
      return backgroundColor;
    currentHit.t = std::numeric_limits<float>::max();
    currentHit.material = nullptr;
    modelview.push(glm::mat4(1.0f));
    root->accept(this);
    modelview.pop();
    if (currentHit.t < std::numeric_limits<float>::max() && currentHit.material)
      return shade(currentHit, ray, bounce);
    return backgroundColor;
  }

  // write the final image buffer to a PPM file
  void writePPM(const std::string &filename) {
    std::ofstream ofs(filename);
    if (!ofs) {
      std::cerr << "Cannot open file: " << filename << std::endl;
      return;
    }
    ofs << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";
    for (int j = 0; j < imageHeight; j++) {
      for (int i = 0; i < imageWidth; i++) {
        glm::vec3 color = imageBuffer[j * imageWidth + i];
        int r = static_cast<int>(255 * std::min(std::max(color.r, 0.0f), 1.0f));
        int g = static_cast<int>(255 * std::min(std::max(color.g, 0.0f), 1.0f));
        int b = static_cast<int>(255 * std::min(std::max(color.b, 0.0f), 1.0f));
        ofs << r << " " << g << " " << b << "  ";
      }
      ofs << "\n";
    }
    ofs.close();
    std::cout << "PPM image written to " << filename << std::endl;
  }
};
} // namespace sgraph

#endif
