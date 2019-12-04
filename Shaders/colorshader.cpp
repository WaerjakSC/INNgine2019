#include "colorshader.h"
#include "innpch.h"
namespace cjk {
ColorShader::ColorShader(cjk::Ref<CameraController> camController, const GLchar *geometryPath)
    : Shader{camController, "PlainShader", geometryPath}
{
    mMatrixUniform = glGetUniformLocation(program, "mMatrix");
    vMatrixUniform = glGetUniformLocation(program, "vMatrix");
    pMatrixUniform = glGetUniformLocation(program, "pMatrix");
}

ColorShader::~ColorShader()
{
    qDebug() << "Deleting ColorShader";
}

} // namespace cjk
