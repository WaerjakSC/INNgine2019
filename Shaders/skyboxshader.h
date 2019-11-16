#ifndef SKYBOXSHADER_H
#define SKYBOXSHADER_H
#include "shader.h"

class SkyboxShader : public Shader {
public:
    SkyboxShader(cjk::Ref<CameraController> camController = nullptr, const GLchar *geometryPath = nullptr);

    void transmitUniformData(gsl::Matrix4x4 &modelMatrix, Material *material) override;

private:
    GLuint skyboxTexUniform{0};
};

#endif // SKYBOXSHADER_H
