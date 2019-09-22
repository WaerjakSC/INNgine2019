#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "pool.h"
#include <memory>
class PhongShader;
class LightSystem {
public:
    LightSystem(PhongShader *shader);

    void update();

    Pool<Light> *lightPool() const;

private:
    std::shared_ptr<Pool<Light>> mLightPool;
    std::shared_ptr<Pool<Transform>> mTransforms;
    PhongShader *mPhong;
};

#endif // LIGHTSYSTEM_H
