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
    Pool<Transform> *transforms = getComponentArray<Transform>().get();
    if (transforms->has(entityID))
        components.push_back(&transforms->get(entityID));
    Pool<Material> *materials = getComponentArray<Material>().get();
    if (materials->has(entityID))
        components.push_back(&materials->get(entityID));
    Pool<Mesh> *meshes = getComponentArray<Mesh>().get();
    if (meshes->has(entityID))
        components.push_back(&meshes->get(entityID));
    Pool<Light> *lights = getComponentArray<Light>().get();
    if (lights->has(entityID))
        components.push_back(&lights->get(entityID));
    Pool<Input> *inputs = getComponentArray<Input>().get();
    if (inputs->has(entityID))
        components.push_back(&inputs->get(entityID));
    Pool<Physics> *physics = getComponentArray<Physics>().get();
    if (physics->has(entityID))
        components.push_back(&physics->get(entityID));
    Pool<Sound> *sound = getComponentArray<Sound>().get();
    if (sound->has(entityID))
        components.push_back(&sound->get(entityID));
    return components;
}
