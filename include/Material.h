#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#define GLM_SWIZZLE
#include <glm/glm.hpp>

namespace util 
{	

    /**
     * This class represents material. Material is represented using (a) ambient (b) diffuse
     * (c) specular (d) emission (for materials that emit light themselves)
     * It also has coefficients for shininess, absorption, reflection (for reflective
     * material), transparency (for transparent material) and refractive index (for
     * transparent material). For the latter ones, the user must make sure that
     * absorption + reflection + transparency = 1
     */
class Material
{
    public:
        Material()
        {
            emission = glm::vec4();
            ambient = glm::vec4();
            diffuse = glm::vec4();
            specular = glm::vec4();
            init();
        }

        Material(const Material& mat)
        {
            emission = glm::vec4(mat.getEmission());
            ambient = glm::vec4(mat.getAmbient());
            diffuse = glm::vec4(mat.getDiffuse());
            specular = glm::vec4(mat.getSpecular());
            this->setShininess(mat.getShininess());
            this->setAbsorption(mat.getAbsorption());
            this->setReflection(mat.getReflection());
            this->setTransparency(mat.getTransparency());
            this->setRefractiveIndex(mat.getRefractiveIndex());
        }

        ~Material(){}

        glm::vec4 getEmission() const
        {
            return emission;
        }


        void setEmission(float r,float g,float b)
        {
            emission.x = r;
            emission.y = g;
            emission.z = b;
            emission.w = 1;
        }
        void setEmission(const glm::vec4& v)
        {
            emission = glm::vec4(v);
        }


        glm::vec4 getAmbient() const
        {
            return ambient;
        }

        void setAmbient(float r,float g,float b)
        {
            ambient.x = r;
            ambient.y = g;
            ambient.z = b;
            ambient.w = 1;
        }
        void setAmbient(const glm::vec4& v)
        {
            ambient = glm::vec4(v);
        }

        glm::vec4 getDiffuse() const
        {
            return diffuse;
        }
        void setDiffuse(float r,float g,float b)
        {
            diffuse.x = r;
            diffuse.y = g;
            diffuse.z = b;
            diffuse.w = 1;
        }

        void setDiffuse(const glm::vec4& v)
        {
            diffuse = glm::vec4(v);
        }

        glm::vec4 getSpecular() const
        {
            return specular;
        }
        void setSpecular(float r,float g,float b)
        {
            specular.x = r;
            specular.y = g;
            specular.z = b;
            specular.w = 1;
        }
        void setSpecular(const glm::vec4& v)
        {
            specular = glm::vec4(v);
        }

        float getShininess() const
        {
            return shininess;
        }
        void setShininess(float r)
        {
            shininess = r;
        }

        float getAbsorption() const
        {
            return absorption;
        }
        void setAbsorption(float a)
        {
            absorption = a;
        }

        float getReflection() const
        {
            return reflection;
        }
        void setReflection(float r)
        {
            reflection = r;
        }

        float getTransparency() const
        {
            return transparency;
        }
        void setTransparency(float t)
        {
            transparency = t;
            ambient.w = diffuse.w = specular.w = emission.w = 1-t;
        }

        float getRefractiveIndex() const
        {
            return refractive_index;
        }
        void setRefractiveIndex(float r)
        {
            refractive_index = r;
        }

    private:
        void init()
        {
            setEmission(0.0f,0.0f,0.0f);
            setAmbient(0.0f,0.0f,0.0f);
            setDiffuse(0.0f,0.0f,0.0f);
            setSpecular(0.0f,0.0f,0.0f);
            setShininess(0.0f);
            setAbsorption(1);
            setReflection(0);
            setTransparency(0);
        }

    private:
        glm::vec4 emission;
        glm::vec4 ambient;
        glm::vec4 diffuse;
        glm::vec4 specular;
        float shininess;
        float absorption,reflection,transparency,refractive_index;
};

}
#endif
