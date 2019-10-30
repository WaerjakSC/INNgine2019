#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "core.h"
#include "isystem.h"
#include "pool.h"
class Registry;
class PhongShader;
class LightSystem : public ISystem {
public:
    LightSystem(Ref<PhongShader> shader);

    void update(float deltaTime = 0.016) override;

    void init(Ref<Entity> light);

private:
    Registry *registry;
    Ref<PhongShader> mPhong;
};

#endif // LIGHTSYSTEM_H
