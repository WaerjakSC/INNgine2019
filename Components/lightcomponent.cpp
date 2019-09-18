#include "lightcomponent.h"
#include "Shaders/shader.h"
LightComponent::LightComponent() {
    light.mObjectColor = gsl::Vector3D{1.f, 1.f, 1.f};
    mType = CType::Light;
}

void LightComponent::update(float dt) {
}

LightData LightComponent::getLight() const {
    return light;
}

void LightComponent::setLightColor(const gsl::Vector3D &color) {
    light.mLightColor = color;
}

//void LightingComponent::draw(gsl::Matrix4x4 &mMatrix) {
//    mShader->transmitUniformData(&mMatrix, &light);
//}
