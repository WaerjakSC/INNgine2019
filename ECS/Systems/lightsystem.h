#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "isystem.h"
#include "pool.h"
#include <memory>
class Registry;
class PhongShader;
class LightSystem : public ISystem {
public:
    LightSystem(PhongShader *shader);

    void update(float deltaTime = 0.016) override;

    void init();

private:
    Registry *registry;
    PhongShader *mPhong;
};

#endif // LIGHTSYSTEM_H
