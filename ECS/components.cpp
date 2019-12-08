#include "components.h"
#include "registry.h"
#include "soundsystem.h"
namespace cjk {

Material::Material(Ref<Shader> shader, GLuint textureUnit, vec3 color, GLfloat specStr, GLint specExp)
    : mSpecularStrength(specStr), mSpecularExponent(specExp), mObjectColor(color),
      mTextureUnit(textureUnit), mShader(shader)
{
    if (!mShader)
        mShader = ResourceManager::instance()->getShader<ColorShader>();
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

} // namespace cjk
