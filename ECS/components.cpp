#include "components.h"
#include "registry.h"
#include "soundsystem.h"

/**
 * @brief Sound constructor - Have to put this in .cpp file to avoid circular dependency with registry
 * @param name Name of the file to be loaded
 * @param loop Whether to loop or play only once
 * @param gain Not sure
 */
Sound::Sound(std::string name, bool loop, float gain)
    : mLooping(loop), mName(name), mGain(gain) {
    alGetError();
    alGenBuffers(1, &mBuffer);
    Registry::instance()->getSystem<SoundSystem>()->checkError("alGenBuffers");
    alGenSources(1, &mSource);
    Registry::instance()->getSystem<SoundSystem>()->checkError("alGenSources");
    alSourcef(mSource, AL_PITCH, 1.0f);
    alSourcef(mSource, AL_GAIN, gain);
}

Material::Material(Ref<Shader> shader, GLuint textureUnit, vec3 color, GLfloat specStr, GLint specExp)
    : mSpecularStrength(specStr), mSpecularExponent(specExp), mObjectColor(color),
      mTextureUnit(textureUnit), mShader(shader) {
    if (!mShader)
        mShader = ResourceManager::instance()->getShader<ColorShader>();
}

AABB::AABB(bool stat) : size(vec3(1.0f, 1.0f, 1.0f)) {
    ResourceManager::instance()->setColliderMesh(colliderMesh);
    isStatic = stat;
}
