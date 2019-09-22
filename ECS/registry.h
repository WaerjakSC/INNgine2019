#ifndef REGISTRY_H
#define REGISTRY_H

#include "pool.h"
#include <memory>
#include <typeinfo>

using ComponentType = std::uint8_t;
// Used to define the size of arrays later on
const ComponentType MAX_COMPONENTS = 32;

class Registry {
public:
    Registry();
    static Registry *instance();
    virtual ~Registry() {}

    /**
     * Register component type. For systems that own the component type
     */
    template <typename Type>
    void registerComponent(std::shared_ptr<Pool<Type>> pool) {
        std::string typeName = typeid(Type).name();
        // Create a ComponentArray pointer and add it to the component arrays map
        mPools.insert({typeName, pool});
    }
    /**
     * For partially owned component types, systems only want a shared ptr to the pool
     * Will register the component type if it exists, otherwise just returns the pool type
     */
    template <typename Type>
    std::shared_ptr<Pool<Type>> registerComponent() {
        std::string typeName = typeid(Type).name();
        std::shared_ptr<Pool<Type>> pool;
        if (mPools.find(typeName) != mPools.end()) {
            pool = getComponentArray<Type>();
        } else {
            pool = std::make_shared<Pool<Type>>();
            // Create a ComponentArray pointer and add it to the component arrays map
            mPools.insert({typeName, pool});
        }
        return pool;
    }
    /**
     * @brief ResourceManager::addComponent - Generic component creator
     * @tparam type Component type enum
     * @param eID Entity ID
     */
    template <typename Type>
    void addComponent(int entityID) {
        // Add a component to the array for an entity
        getComponentArray<Type>()->add(entityID);
    }

    template <typename Type>
    void removeComponent(int entityID) {
        // Remove a component from the array for an entity
        getComponentArray<Type>()->remove(entityID);
    }

    template <typename Type>
    Type &getComponent(int entityID) {
        // Get a reference to a component from the array for an entity
        return getComponentArray<Type>()->get(entityID);
    }
    template <typename Type>
    Type &getLastComponent() {
        // Get a reference to a component from the array for an entity
        return getComponentArray<Type>()->getLast();
    }
    void entityDestroyed(int entityID);

    std::vector<Component *> getComponents(int entityID);

private:
    static Registry *mInstance;
    std::map<std::string, std::shared_ptr<IPool>> mPools{};

    // Convenience function to get the statically casted pointer to the ComponentArray of type T.
    template <typename Type>
    std::shared_ptr<Pool<Type>> getComponentArray() {
        std::string typeName = typeid(Type).name();
        return std::static_pointer_cast<Pool<Type>>(mPools[typeName]);
    }
};

#endif // REGISTRY_H
