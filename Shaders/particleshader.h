#ifndef PARTICLESHADER_H
#define PARTICLESHADER_H

#include "shader.h"

struct ParticleEmitter;

class ParticleShader : public Shader {
    using vec3 = gsl::Vector3D;

public:
    ParticleShader(cjk::Ref<CameraController> camController = nullptr, const GLchar *geometryPath = nullptr);

    void transmitParticleUniformData(const ParticleEmitter &emitter);

private:
    GLuint textureUniform{0}, cameraRightUniform{0}, cameraUpUniform{0};
};

#endif // PARTICLESHADER_H
