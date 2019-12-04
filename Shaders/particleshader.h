#ifndef PARTICLESHADER_H
#define PARTICLESHADER_H

#include "shader.h"
namespace cjk {
struct ParticleEmitter;

class ParticleShader : public Shader {
public:
    ParticleShader(cjk::Ref<CameraController> camController = nullptr, const GLchar *geometryPath = nullptr);

    void transmitParticleUniformData(const ParticleEmitter &emitter);

private:
    GLuint textureUniform{0}, cameraRightUniform{0}, cameraUpUniform{0};
};
} // namespace cjk

#endif // PARTICLESHADER_H
