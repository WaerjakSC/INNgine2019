#ifndef COLORSHADER_H
#define COLORSHADER_H

#include "shader.h"
namespace cjk {
class ColorShader : public Shader {
public:
    ColorShader(cjk::Ref<CameraController> camController = nullptr, const GLchar *geometryPath = nullptr);
    virtual ~ColorShader() override;
};
} // namespace cjk

#endif // COLORSHADER_H
