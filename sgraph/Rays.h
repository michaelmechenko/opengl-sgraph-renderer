#ifndef RAYS_H
#define RAYS_H

#include <Material.h>
#include <glm/glm.hpp>
#include <memory>

// Textures are unimplemented for this project
class Texture;

// Represents a 3D ray with an origin and a direction
struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;

  Ray(const glm::vec3 &origin, const glm::vec3 &direction)
      : origin(origin), direction(glm::normalize(direction)) {}
};

// Stores information about a ray-object intersection
struct HitRecord {
  float t;          // Ray parameter t at intersection
  glm::vec3 point;  // Intersection point in view coordinates
  glm::vec3 normal; // Normal at the intersection point
  std::shared_ptr<util::Material>
      material;        // Material properties of the intersected object
  glm::vec2 texCoords; // Texture coordinates
  std::shared_ptr<Texture> texture; // Pointer to texture, if applicable

  HitRecord()
      : t(0.0f), point(glm::vec3(0.0f)), normal(glm::vec3(0.0f)),
        texCoords(glm::vec2(0.0f)) {}
};

#endif
