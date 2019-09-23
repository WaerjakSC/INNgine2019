#ifndef REGISTRY_H
#define REGISTRY_H

#include "pool.h"
#include <memory>
#include <typeinfo>

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
     * @brief Adds an entity and its new component to a pool of that type.
     * Entity is equivalent to Component in this case, since a pool won't contain the entity if the entity doesn't have the component.
     * @example the Transform component type can take 3 extra variables, add<Transform>(entityID, position, rotation, scale) will initialize its variables to custom values.
     * @param entityID
     * @tparam Args... args Variadic parameter pack - Use as many function parameters as needed to construct the component.
     */
    template <typename Type, class... Args>
    void addComponent(int entityID, Args... args) {
        // Add a component to the array for an entity
        getComponentArray<Type>()->add(entityID, args...);
    }
    /**
     * @brief Remove a component of type Type from the entity/gameobject with entityID.
     */
    template <typename Type>
    void removeComponent(int entityID) {
        // Remove a component from the array for an entity
        getComponentArray<Type>()->remove(entityID);
    }
    /**
     * @brief Get a reference to the Type component owned by entityID
     */
    template <typename Type>
    Type &getComponent(int entityID) {
        // Get a reference to a component from the array for an entity
        return getComponentArray<Type>()->get(entityID);
    }
    /**
     * @brief get a reference to the last component created of that Type, if you don't have or don't need the entityID
     */
    template <typename Type>
    Type &getLastComponent() {
        // Get a reference to a component from the array for an entity
        return getComponentArray<Type>()->getLast();
    }
    /**
     * @brief entityDestroyed is called when an entity is removed from the game.
     * Iterates through all the Pools, and if they contain a component owned by entityID, delete and re-arrange the Pool.
     * @param entityID
     */
    void entityDestroyed(int entityID);

    /**
     * @brief Temporary function for MainWindow to get all the components owned by an entity.
     * Has to be a better way to do this.
     * @param entityID
     * @return
     */
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
