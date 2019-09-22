#include "material.h"

Material::Material() {
    mType = CType::Material;
}

void Material::update(float dt) {
}

void Material::setShader(ShaderType shader) {
    mShader = shader;
}
void Material::setColor(const gsl::Vector3D &color) {
    material.mObjectColor = color;
}

ShaderType Material::getShader() const {
    return mShader;
}

void Material::setTextureUnit(const GLuint &textureUnit) {
    material.mTextureUnit = textureUnit;
}
