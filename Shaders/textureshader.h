#ifndef TEXTURESHADER_H
#define TEXTURESHADER_H

#include "shader.h"

struct Material;

class TextureShader : public Shader {
public:
    TextureShader(cjk::Ref<CameraController> camController = nullptr, const GLchar *geometryPath = nullptr);
    virtual ~TextureShader() override;

    void transmitUniformData(gsl::Matrix4x4 &modelMatrix, Material *material) override;

private:
    GLint objectColorUniform{-1};
    GLint textureUniform{-1};
};


#endif // TEXTURESHADER_H
