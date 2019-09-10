#include "materialcomponent.h"

MaterialComponent::MaterialComponent() {
    mType = CType::Material;
}

void MaterialComponent::update(float dt) {
}

void MaterialComponent::setShader(ShaderType shader) {
    mShader = shader;
}
void MaterialComponent::setColor(const gsl::Vector3D &color) {
    material.mObjectColor = color;
}

ShaderType MaterialComponent::getShader() const {
    return mShader;
}

void MaterialComponent::setTextureUnit(const GLuint &textureUnit) {
    material.mTextureUnit = textureUnit;
}
