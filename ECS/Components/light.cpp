#include "light.h"
#include "shader.h"
Light::Light() {
    light.mObjectColor = gsl::Vector3D{1.f, 1.f, 1.f};
    mType = CType::Light;
}

void Light::update(float dt) {
}

LightData Light::getLight() const {
    return light;
}

void Light::setLightColor(const gsl::Vector3D &color) {
    light.mLightColor = color;
}

//void LightingComponent::draw(gsl::Matrix4x4 &mMatrix) {
//    mShader->transmitUniformData(&mMatrix, &light);
//}
