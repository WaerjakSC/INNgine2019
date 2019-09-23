#include "lightsystem.h"
#include "phongshader.h"
#include "registry.h"

LightSystem::LightSystem(PhongShader *shader)
    : mPhong(shader) {
    mTransforms = Registry::instance()->registerComponent<Transform>();
    mLightPool = std::make_shared<Pool<Light>>();
    Registry::instance()->registerComponent<Light>(mLightPool);
}
void LightSystem::update() {
    GLuint curEntity{0}; // Get the actual ID.
    for (auto entityID : mLightPool->entities()) {
        // Send the entity's model matrix as well as the light data.
        mPhong->updateLightUniforms(&mTransforms->get(entityID).matrix(), mLightPool->data()[curEntity].getLight());
        curEntity++;
    }
}

Pool<Light> *LightSystem::lightPool() const {
    return mLightPool.get();
}
