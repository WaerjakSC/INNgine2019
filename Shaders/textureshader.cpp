#include "textureshader.h"
#include "components.h"
#include "innpch.h"
namespace cjk {
TextureShader::TextureShader(cjk::Ref<CameraController> camController, const GLchar *geometryPath)
    : Shader{camController, "TextureShader", geometryPath}
{
    mMatrixUniform = glGetUniformLocation(program, "mMatrix");
    vMatrixUniform = glGetUniformLocation(program, "vMatrix");
    pMatrixUniform = glGetUniformLocation(program, "pMatrix");
    objectColorUniform = glGetUniformLocation(program, "objectColor");
    textureUniform = glGetUniformLocation(program, "textureSampler");
}

TextureShader::~TextureShader()
{
    qDebug() << "Deleting TextureShader";
}

void TextureShader::transmitUniformData(gsl::Matrix4x4 &modelMatrix, Material *material)
{
    Shader::transmitUniformData(modelMatrix);

    glUniform1i(textureUniform, material->mTextureUnit); //TextureUnit = 0 as default);
    glUniform3f(objectColorUniform, material->mObjectColor.x, material->mObjectColor.y, material->mObjectColor.z);
}
} // namespace cjk
