#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "pool.h"
#include <memory>
class PhongShader;
class LightSystem {
public:
    LightSystem(PhongShader *shader);

    void update();

    Pool<LightComponent> *lightPool() const;

private:
    std::shared_ptr<Pool<LightComponent>> mLightPool;
    std::shared_ptr<Pool<TransformComponent>> mTransforms;
    PhongShader *mPhong;
};

#endif // LIGHTSYSTEM_H
