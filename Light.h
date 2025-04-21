#ifndef ___LIGHT_H___
#define ___LIGHT_H___

#include "glm/glm.hpp"

namespace util {

/**
 * This class represents a single light source.
 * The light source has various properties: position (location or direction),
 * colors (ambient, diffuse, specular)
 * direction in which it is pointing (if a spotlight), angle of the cone (if a
 * spotlight)
 */
class Light {
public:
  Light() {
    ambient = glm::vec3(0, 0, 0);
    diffuse = glm::vec3(0, 0, 0);
    specular = glm::vec3(0, 0, 0);
    position = glm::vec4(0, 0, 0, 1);

    spotDirection = glm::vec4(0, 0, -1, 0);
    spotCutoff = 0.0f;
  }
  Light(const Light &l) {
    ambient = glm::vec3(l.ambient);
    diffuse = glm::vec3(l.diffuse);
    specular = glm::vec3(l.specular);

    position = glm::vec4(l.position);
    spotDirection = glm::vec4(l.spotDirection);
    spotCutoff = l.spotCutoff;
  }
  ~Light() {}

  inline glm::vec3 getAmbient() const;
  inline void setAmbient(float r, float g, float b);
  inline void setAmbient(const glm::vec3 &amb);

  inline glm::vec3 getDiffuse() const;
  inline void setDiffuse(float r, float g, float b);
  inline void setDiffuse(const glm::vec3 &diff);

  inline glm::vec3 getSpecular() const;
  inline void setSpecular(float r, float g, float b);
  inline void setSpecular(const glm::vec3 &spec);

  inline glm::vec4 getPosition() const;
  inline void setPosition(const glm::vec4 &pos);
  inline void setPosition(float x, float y, float z);
  inline void setDirection(float x, float y, float z);

  inline glm::vec4 getSpotDirection() const;
  inline void setSpotDirection(float x, float y, float z);

  inline float getSpotCutoff() const;
  inline void setSpotAngle(float angle);

private:
  glm::vec3 ambient, diffuse, specular;
  glm::vec4 position, spotDirection;
  float spotCutoff;
};

void Light::setAmbient(float r, float g, float b) {
  ambient = glm::vec3(r, g, b);
}

void Light::setDiffuse(float r, float g, float b) {
  diffuse = glm::vec3(r, g, b);
}

void Light::setSpecular(float r, float g, float b) {
  specular = glm::vec3(r, g, b);
}

void Light::setPosition(float x, float y, float z) {
  position = glm::vec4(x, y, z, 1.0f);
}

void Light::setDirection(float x, float y, float z) {
  position = glm::vec4(x, y, z, 0.0f);
}

void Light::setSpotDirection(float x, float y, float z) {
  spotDirection = glm::vec4(x, y, z, 0.0f);
}

void Light::setAmbient(const glm::vec3 &amb) { ambient = glm::vec3(amb); }

void Light::setDiffuse(const glm::vec3 &diff) { diffuse = glm::vec3(diff); }

void Light::setSpecular(const glm::vec3 &spec) { specular = glm::vec3(spec); }

void Light::setSpotAngle(float angle) { spotCutoff = angle; }

void Light::setPosition(const glm::vec4 &pos) { position = glm::vec4(pos); }

glm::vec3 Light::getAmbient() const { return glm::vec3(ambient); }

glm::vec3 Light::getDiffuse() const { return glm::vec3(diffuse); }

glm::vec3 Light::getSpecular() const { return glm::vec3(specular); }

glm::vec4 Light::getPosition() const { return glm::vec4(position); }

glm::vec4 Light::getSpotDirection() const { return glm::vec4(spotDirection); }

float Light::getSpotCutoff() const { return spotCutoff; }
} // namespace util
#endif
