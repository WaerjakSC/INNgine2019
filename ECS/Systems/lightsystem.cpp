#include "lightsystem.h"
#include "phongshader.h"
#include "registry.h"

LightSystem::LightSystem(PhongShader *shader)
    : mPhong(shader) {
    mTransforms = Registry::instance()->registerComponent<TransformComponent>();
    mLightPool = Registry::instance()->registerComponent<LightComponent>();
}
void LightSystem::update() {
    GLuint curEntity{0}; // Get the actual ID.
    for (auto entityID : mLightPool->entities()) {
        // Send the entity's model matrix as well as the light data.
        mPhong->updateLightUniforms(&mTransforms->get(entityID).matrix(), mLightPool->data()[curEntity].getLight());
        curEntity++;
    }
}

Pool<LightComponent> *LightSystem::lightPool() const {
    return mLightPool.get();
}
