#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "core.h"
#include "isystem.h"

class Registry;
class PhongShader;
class LightSystem : public ISystem {
public:
    LightSystem(cjk::Ref<PhongShader> shader);

    void update(DeltaTime = 0.016) override;

    void init();

private:
    Registry *registry;
    cjk::Ref<PhongShader> mPhong;
};

#endif // LIGHTSYSTEM_H
