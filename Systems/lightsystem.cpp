#include "lightsystem.h"
#include "phongshader.h"

LightSystem::LightSystem(Pool<LightComponent> *lights, Pool<TransformComponent> *transforms, PhongShader *shader)
    : mLightPool(lights), mTransforms(transforms), mPhong(shader) {
}
void LightSystem::update() {
    for (size_t i = 0; i < mLightPool->size(); i++) {
        int curEntity = mLightPool->getEntityList().at(i); // Get the actual ID.
        // Send the entity's model matrix as well as the light data.
        mPhong->updateLightUniforms(&mTransforms->get(curEntity)->matrix(), mLightPool->getComponents().at(i)->getLight());
    }
}
