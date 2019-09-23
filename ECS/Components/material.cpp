#include "material.h"

Material::Material() {
    mType = CType::Material;
}

Material::Material(ShaderType type, GLuint texture, gsl::Vector3D color) : Material(type, MaterialData(color, texture)) {}

Material::Material(ShaderType type, MaterialData data) : material(data), mShader(type) {
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
