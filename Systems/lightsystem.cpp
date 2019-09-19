#include "lightsystem.h"
#include "phongshader.h"

LightSystem::LightSystem(std::shared_ptr<Pool<TransformComponent>> transforms, PhongShader *shader)
    : mTransformPool(transforms), mPhong(shader) {
    mLightPool = std::make_unique<Pool<LightComponent>>();
}
void LightSystem::update() {
    for (size_t i = 0; i < mLightPool->size(); i++) {
        int curEntity = mLightPool->getEntityList().at(i); // Get the actual ID.
        // Send the entity's model matrix as well as the light data.
        mPhong->updateLightUniforms(&mTransformPool->get(curEntity)->matrix(), mLightPool->getComponents().at(i)->getLight());
    }
}

Pool<LightComponent> *LightSystem::lightPool() const {
    return mLightPool.get();
}
