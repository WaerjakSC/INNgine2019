#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "pool.h"
#include <memory>
class PhongShader;
class LightSystem {
public:
    LightSystem(std::shared_ptr<Pool<TransformComponent>> transforms, PhongShader *shader);

    void update();

    Pool<LightComponent> *lightPool() const;

private:
    std::unique_ptr<Pool<LightComponent>> mLightPool;
    std::shared_ptr<Pool<TransformComponent>> mTransformPool;
    PhongShader *mPhong;
};

#endif // LIGHTSYSTEM_H