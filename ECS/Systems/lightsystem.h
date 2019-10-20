#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "pool.h"
#include "isystem.h"
#include <memory>
class PhongShader;
class LightSystem : public ISystem {
public:
    LightSystem(PhongShader *shader);

    void update(float deltaTime = 0.016) override;

    Pool<Light> *lightPool() const;

private:
    std::shared_ptr<Pool<Light>> mLightPool;
    std::shared_ptr<Pool<Transform>> mTransforms;
    PhongShader *mPhong;
};

#endif // LIGHTSYSTEM_H
