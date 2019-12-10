#ifndef SHADER_H
#define SHADER_H

#include "core.h"
#include <QOpenGLFunctions_4_1_Core>

class CameraController;
namespace gsl {
class Matrix4x4;
}
struct Material;
class Shader : protected QOpenGLFunctions_4_1_Core {
public:
    // Constructor generates the shader on the fly
    Shader(cjk::Ref<CameraController> camController = nullptr, const std::string shaderName = "PlainShader", const GLchar *geometryPath = nullptr);
    virtual ~Shader();

    // Use the current shader
    void use();

    //Get program number for this shader
    GLuint getProgram() const;
    virtual void transmitUniformData(gsl::Matrix4x4 &modelMatrix, Material *material = nullptr);

    void setCameraController(cjk::Ref<CameraController> currentController);

    cjk::Ref<CameraController> getCameraController() const;

    std::string getName() const;

protected:
    GLuint program{0};
    GLint mMatrixUniform{-1};
    GLint vMatrixUniform{-1};
    GLint pMatrixUniform{-1};
    std::string mName;

    cjk::Ref<CameraController> mCameraController;
};

#endif
