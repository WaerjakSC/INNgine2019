#include "registry.h"

Registry *Registry::mInstance = nullptr;

Registry::Registry() {
}
Registry *Registry::instance() {
    if (!mInstance)
        mInstance = new Registry();
    return mInstance;
}
void Registry::entityDestroyed(int entityID) {
    // Notify each component array that an entity has been destroyed.
    // If it has a component for that entity, it will remove it.
    for (auto const &pair : mPools) {
        auto const &component = pair.second;

        component->remove(entityID);
    }
}
/**
 * @brief Too tired to figure out how to do this in a smart way
 * @param entityID
 * @return
 */
std::vector<Component *> Registry::getComponents(int entityID) {
    std::vector<Component *> components;
    Pool<TransformComponent> *transforms = getComponentArray<TransformComponent>().get();
    if (transforms->has(entityID))
        components.push_back(&transforms->get(entityID));
    Pool<MaterialComponent> *materials = getComponentArray<MaterialComponent>().get();
    if (materials->has(entityID))
        components.push_back(&materials->get(entityID));
    Pool<MeshComponent> *meshes = getComponentArray<MeshComponent>().get();
    if (meshes->has(entityID))
        components.push_back(&meshes->get(entityID));
    Pool<LightComponent> *lights = getComponentArray<LightComponent>().get();
    if (lights->has(entityID))
        components.push_back(&lights->get(entityID));
    Pool<InputComponent> *inputs = getComponentArray<InputComponent>().get();
    if (inputs->has(entityID))
        components.push_back(&inputs->get(entityID));
    Pool<PhysicsComponent> *physics = getComponentArray<PhysicsComponent>().get();
    if (physics->has(entityID))
        components.push_back(&physics->get(entityID));
    Pool<SoundComponent> *sound = getComponentArray<SoundComponent>().get();
    if (sound->has(entityID))
        components.push_back(&sound->get(entityID));
    return components;
}
