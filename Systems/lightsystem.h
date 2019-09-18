#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "pool.h"

class PhongShader;
class LightSystem {
public:
    LightSystem(Pool<LightComponent> *lights, Pool<TransformComponent> *transforms, PhongShader *shader);

    void update();

private:
    Pool<LightComponent> *mLightPool;
    Pool<TransformComponent> *mTransforms;
    PhongShader *mPhong;
};

#endif // LIGHTSYSTEM_H
