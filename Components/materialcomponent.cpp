#include "materialcomponent.h"
#include "Shaders/shader.h"

MaterialComponent::MaterialComponent() {
    mType = CType::Material;
}

void MaterialComponent::update(float dt) {
}

void MaterialComponent::draw(gsl::Matrix4x4 &mMatrix) {
    mShader->transmitUniformData(&mMatrix, &material);
}

void MaterialComponent::setShader(Shader *shader) {
    mShader = shader;
}
void MaterialComponent::setColor(const gsl::Vector3D &color) {
    material.mObjectColor = color;
}

Shader *MaterialComponent::getShader() const {
    return mShader;
}

void MaterialComponent::setTextureUnit(const GLuint &textureUnit) {
    material.mTextureUnit = textureUnit;
}
