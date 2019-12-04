#include "lightsystem.h"
#include "phongshader.h"
#include "registry.h"
namespace cjk {
LightSystem::LightSystem(Ref<PhongShader> shader)
    : registry{Registry::instance()}, mPhong{shader}
{
}
void LightSystem::update(DeltaTime)
{
    //    auto view = registry->view<Transform, Light>();
    //    for (auto entityID : view) {
    //        auto [transform, light] = view.get<Transform, Light>(entityID);
    //        // Send the entity's model matrix as well as the light data.
    //        mPhong->updateLightUniforms(transform.modelMatrix, light.mLight);
    //    }
}
void LightSystem::init()
{
    update();
}
} // namespace cjk
