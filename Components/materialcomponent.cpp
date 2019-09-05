#include "materialcomponent.h"
#include "Shaders/shader.h"
MaterialComponent::MaterialComponent() {
}

void MaterialComponent::update(float dt) {
    mShader->transmitUniformData()
}

void MaterialComponent::setShader(Shader *shader) {
    mShader = shader;
}
void MaterialComponent::setColor(const gsl::Vector3D &color) {
    mObjectColor = color;
}

void MaterialComponent::setTextureUnit(const GLuint &textureUnit) {
    mTextureUnit = textureUnit;
}
