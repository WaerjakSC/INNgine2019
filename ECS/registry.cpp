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
