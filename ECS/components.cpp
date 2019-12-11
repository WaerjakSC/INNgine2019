#include "components.h"
#include "colorshader.h"
#include "resourcemanager.h"

Material::Material(cjk::Ref<Shader> shaderIn, GLuint texUnit, vec3 color, GLfloat specStr, GLint specExp)
    : specularStrength(specStr), specularExponent(specExp), objectColor(color),
      textureUnit(texUnit), shader(shaderIn)
{
    if (!shaderIn)
        shader = ResourceManager::instance()->getShader<ColorShader>();
}

AABB::AABB(bool stat) : size(vec3(1.0f, 1.0f, 1.0f))
{
    ResourceManager::instance()->setAABBMesh(colliderMesh);
    isStatic = stat;
}

void ParticleEmitter::setNumParticles(size_t num)
{
    numParticles = num;
    positionData = std::vector<GLfloat>(numParticles * 4);
    colorData = std::vector<GLubyte>(numParticles * 4);
    particles = std::vector<Particle>(numParticles);
    ResourceManager::instance()->initParticleEmitter(*this);
}
