#include "material.h"
#include "Shaders/textureshader.h"
#include "innpch.h"

Material::Material()
{
}

void Material::setColor(const gsl::Vector3D &color)
{
    mObjectColor = color;
}

void Material::setTextureUnit(const GLuint &textureUnit)
{
    mTextureUnit = textureUnit;
}

void Material::setShader(Shader *shader)
{
    mShader = shader;
}
