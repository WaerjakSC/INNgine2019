#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "isystem.h"
#include "pool.h"
#include <memory>
class PhongShader;
class LightSystem : public ISystem {
public:
    LightSystem(PhongShader *shader);

    void update(float deltaTime = 0.016) override;

    Pool<Light> *lightPool() const;

    void init();

private:
    std::shared_ptr<Pool<Light>> mLightPool;
    std::shared_ptr<Pool<Transform>> mTransforms;
    PhongShader *mPhong;
};

#endif // LIGHTSYSTEM_H
