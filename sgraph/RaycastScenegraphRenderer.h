#ifndef _RAYCASTSCENEGRAPHRENDERER_H_
#define _RAYCASTSCENEGRAPHRENDERER_H_

// Standard and third-party includes
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

/**
 * @brief Class to render a scene graph using raycasting.
 *
 * This class traverses a scene graph and casts rays through each pixel,
 * computes intersections with objects, shades the hit points, and writes
 * the final image as a PPM file.
 */
class RaycastScenegraphRenderer : public SGNodeVisitor {
public:
  /**
   * @brief Constructor for the renderer.
   *
   * @param mv Stack of modelview matrices.
   * @param os Map of object instances.
   * @param width Image width.
   * @param height Image height.
   */
  RaycastScenegraphRenderer(std::stack<glm::mat4> &mv,
                            std::map<std::string, util::ObjectInstance *> &os,
                            int width, int height)
      : modelview(mv), objects(os), imageWidth(width), imageHeight(height) {
    imageBuffer.resize(imageWidth * imageHeight, glm::vec3(0.0f));
    backgroundColor = glm::vec3(0.0f);
    viewPlaneZ = -1.0f;
  }

  /**
   * @brief Render the scene graph and output the resulting image to a PPM file.
   *
   * Traverses the scene graph, casts rays for each pixel, computes shading,
   * and writes the final image to the specified output file.
   *
   * @param root Pointer to the root node of the scene graph.
   * @param outputFile The file name to write the PPM image.
   */
  void render(SGNode *root, const std::string &outputFile) {
    this->root = root;
    glm::mat4 viewTransform = modelview.top();
    glm::mat4 invView = glm::inverse(viewTransform);
    glm::vec3 eye = glm::vec3(invView * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Loop over each pixel in the image
    for (int j = 0; j < imageHeight; j++) {
      for (int i = 0; i < imageWidth; i++) {
        // Convert pixel coordinates to normalized device coordinates (NDC)
        float ndcX = (2.0f * i) / (imageWidth - 1) - 1.0f;
        float ndcY = 1.0f - (2.0f * j) / (imageHeight - 1);
        glm::vec3 pixelPoint(ndcX, ndcY, viewPlaneZ);
        // Transform pixel to world coordinates
        glm::vec3 worldPixel = glm::vec3(invView * glm::vec4(pixelPoint, 1.0f));
        // Compute ray direction from eye to pixel point
        glm::vec3 rayDir = glm::normalize(worldPixel - eye);
        currentRay = Ray(eye, rayDir);

        // Reset hit record before casting a new ray
        currentHit.t = std::numeric_limits<float>::max();
        currentHit.material = nullptr;

        // Initialize the modelview for this ray
        modelview.push(glm::mat4(1.0f));
        // Traverse the scene graph
        root->accept(this);
        modelview.pop();

        // Determine the pixel color based on ray hit
        glm::vec3 pixelColor = backgroundColor;
        if (currentHit.t < std::numeric_limits<float>::max() &&
            currentHit.material)
          pixelColor = shade(currentHit, currentRay, maxBounce);
        imageBuffer[j * imageWidth + i] = pixelColor;
      }
    }
    // Write the computed image to the output file
    writePPM(outputFile);
  }

  /**
   * @brief Visit a group node in the scene graph.
   *
   * Recursively visits all child nodes of the group node.
   *
   * @param groupNode Pointer to the group node.
   */
  virtual void visitGroupNode(GroupNode *groupNode) {
    for (size_t i = 0; i < groupNode->getChildren().size(); i++) {
      groupNode->getChildren()[i]->accept(this);
    }
  }

  /**
   * @brief Visit a leaf node in the scene graph.
   *
   * Tests for intersections with the object instance contained in the leaf
   * node.
   *
   * @param leafNode Pointer to the leaf node.
   */
  virtual void visitLeafNode(LeafNode *leafNode) {
    std::string instanceName = leafNode->getInstanceOf();
    // Create a shared pointer for the material
    std::shared_ptr<util::Material> material =
        std::make_shared<util::Material>(leafNode->getMaterial());
    glm::mat4 M = modelview.top();
    glm::mat4 invM = glm::inverse(M);
    // Transform the current ray to the local coordinate system
    Ray localRay = transformRay(currentRay, invM);
    HitRecord localHit;
    localHit.t = std::numeric_limits<float>::max();
    bool hit = false;
    // Determine the type of object and test for intersection
    if (instanceName.find("box") != std::string::npos) {
      hit = intersectBox(localRay, localHit);
    } else if (instanceName.find("sphere") != std::string::npos) {
      hit = intersectSphere(localRay, localHit);
    }
    // If there is an intersection update the current hit record
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

  /**
   * @brief Visit a transform node in the scene graph.
   *
   * Pushes the current modelview, applies the transform, visits children,
   * and then restores the previous modelview.
   *
   * @param transformNode Pointer to the transform node.
   */
  virtual void visitTransformNode(TransformNode *transformNode) {
    modelview.push(modelview.top());
    modelview.top() = modelview.top() * transformNode->getTransform();
    for (size_t i = 0; i < transformNode->getChildren().size(); i++) {
      transformNode->getChildren()[i]->accept(this);
    }
    modelview.pop();
  }

  /**
   * @brief Visit a scale transform node.
   *
   * Uses the generic transform node visit function.
   *
   * @param scaleNode Pointer to the scale transform node.
   */
  virtual void visitScaleTransform(ScaleTransform *scaleNode) {
    visitTransformNode(scaleNode);
  }

  /**
   * @brief Visit a translate transform node.
   *
   * Uses the generic transform node visit function.
   *
   * @param translateNode Pointer to the translate transform node.
   */
  virtual void visitTranslateTransform(TranslateTransform *translateNode) {
    visitTransformNode(translateNode);
  }

  /**
   * @brief Visit a rotate transform node.
   *
   * Uses the generic transform node visit function.
   *
   * @param rotateNode Pointer to the rotate transform node.
   */
  virtual void visitRotateTransform(RotateTransform *rotateNode) {
    visitTransformNode(rotateNode);
  }

private:
  // Reference to the current modelview matrix stack.
  std::stack<glm::mat4> &modelview;
  // Map of object instances for rendering.
  std::map<std::string, util::ObjectInstance *> &objects;
  // Dimensions of the output image.
  int imageWidth, imageHeight;
  // Buffer to store pixel colors.
  std::vector<glm::vec3> imageBuffer;
  // Background color for the scene.
  glm::vec3 backgroundColor;
  // Z-coordinate for the view plane.
  float viewPlaneZ;
  // Maximum number of bounces for reflection rays.
  int maxBounce = 5;
  // Pointer to the root of the scene graph.
  SGNode *root;
  // Current ray being cast.
  Ray currentRay = Ray(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
  // Record of the closest hit.
  HitRecord currentHit;

  /**
   * @brief Transform a ray using a transformation matrix.
   *
   * Applies the matrix transformation to the ray's origin and direction.
   *
   * @param ray The original ray.
   * @param mat The transformation matrix.
   * @return The transformed ray.
   */
  Ray transformRay(const Ray &ray, const glm::mat4 &mat) {
    glm::vec4 newOrigin = mat * glm::vec4(ray.origin, 1.0f);
    glm::vec4 newDir = mat * glm::vec4(ray.direction, 0.0f);
    return Ray(glm::vec3(newOrigin) / newOrigin.w,
               glm::normalize(glm::vec3(newDir)));
  }

  /**
   * @brief Check for intersection between the ray and a sphere.
   *
   * Assumes a sphere of radius 1 centered at the origin.
   *
   * @param ray The ray in local coordinates.
   * @param hit Reference to the hit record to store intersection details.
   * @return True if an intersection occurs, false otherwise.
   */
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

  /**
   * @brief Check for intersection between the ray and a box.
   *
   * Assumes an axis-aligned box bounded by [-0.5, 0.5] along each axis.
   *
   * @param ray The ray in local coordinates.
   * @param hit Reference to the hit record to store intersection details.
   * @return True if an intersection occurs, false otherwise.
   */
  bool intersectBox(const Ray &ray, HitRecord &hit) {
    glm::vec3 minB(-0.5f), maxB(0.5f);
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();
    // Iterate over the three dimensions
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
    // Determine which face of the box was hit based on the offset
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

  /**
   * @brief Compute shading for a hit point by considering lighting and
   * reflections.
   *
   * Computes ambient, diffuse, specular components and handles reflection by
   * recursively tracing a reflection ray.
   *
   * @param hit The hit record containing intersection details.
   * @param ray The incoming ray.
   * @param bounce Remaining bounce count for reflections.
   * @return The computed color for the hit point.
   */
  glm::vec3 shade(const HitRecord &hit, const Ray &ray, int bounce) {
    const float epsilon = 1e-3f;
    glm::vec3 ambient = glm::vec3(hit.material->getAmbient());
    glm::vec3 color = ambient;

    // Define light positions and intensities
    std::vector<glm::vec3> lightPositions = {glm::vec3(0, 0, 30),
                                             glm::vec3(0, 10, 0)};
    std::vector<glm::vec3> lightIntensities = {glm::vec3(0.5f),
                                               glm::vec3(0.5f)};

    // Process each light source for diffuse and specular lighting
    for (size_t i = 0; i < lightPositions.size(); i++) {
      glm::vec3 L = glm::normalize(lightPositions[i] - hit.point);
      // Offset the shadow ray start to prevent self-intersection
      Ray shadowRay(hit.point + epsilon * hit.normal, L);
      HitRecord shadowHit;
      shadowHit.t = std::numeric_limits<float>::max();

      // Determine if the point is in shadow with respect to the current light
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

    // Handle reflections if applicable
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

  /**
   * @brief Helper function to perform element-wise multiplication (Hadamard
   * product) on two vectors.
   *
   * @param a The first vector.
   * @param b The second vector.
   * @return The resulting vector after element-wise multiplication.
   */
  inline glm::vec3 HadamardProduct(const glm::vec3 &a, const glm::vec3 &b) {
    return glm::vec3(a.x * b.x, a.y * b.y, a.z * b.z);
  }

  /**
   * @brief Recursively trace a ray for reflections.
   *
   * Traces the ray through the scene graph and computes the color contribution.
   *
   * @param ray The ray to trace.
   * @param bounce Remaining bounce count.
   * @return The computed color from the traced ray.
   */
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

  /**
   * @brief Write the image buffer to a PPM file.
   *
   * Outputs the final image computed from the raycasting process to a PPM file.
   *
   * @param filename The name of the output file.
   */
  void writePPM(const std::string &filename) {
    std::ofstream ofs(filename);
    if (!ofs) {
      std::cerr << "Cannot open file: " << filename << std::endl;
      return;
    }
    ofs << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";
    // Write each pixel's RGB values to the file in PPM format
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
