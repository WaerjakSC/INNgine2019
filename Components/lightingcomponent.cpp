#include "lightingcomponent.h"
#include "Shaders/shader.h"
LightingComponent::LightingComponent() {
    light.mObjectColor = gsl::Vector3D{1.f, 1.f, 1.f};
    mType = CType::Light;
}

//void LightingComponent::draw(gsl::Matrix4x4 &mMatrix) {
//    mShader->transmitUniformData(&mMatrix, &light);
//}
//void LightingComponent::setColor(const gsl::Vector3D &color) {
//    material.mObjectColor = color;
//    light.mObjectColor = color;
//}

//void LightingComponent::draw(gsl::Matrix4x4 &mMatrix) {
//    mShader->transmitUniformData(&mMatrix, &light);
//}
//void LightingComponent::setColor(const gsl::Vector3D &color) {
//    material.mObjectColor = color;
//    light.mObjectColor = color;
//}
