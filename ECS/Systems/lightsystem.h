#ifndef LIGHTSYSTEM_H
#define LIGHTSYSTEM_H
#include "core.h"
#include "isystem.h"
#include "pool.h"
namespace cjk {
class Registry;
class PhongShader;
class LightSystem : public ISystem {
public:
    LightSystem(Ref<PhongShader> shader);

    void update(DeltaTime = 0.016) override;

    void init();

private:
    Registry *registry;
    Ref<PhongShader> mPhong;
};

} // namespace cjk

#endif // LIGHTSYSTEM_H
