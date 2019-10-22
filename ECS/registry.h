#ifndef REGISTRY_H
#define REGISTRY_H

#include "entity.h"
#include "isystem.h"
#include "pool.h"
#include "resourcemanager.h"
#include <memory>
#include <typeinfo>

class Registry : public QObject {
    Q_OBJECT
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
    template <typename Type, class... Args>
    std::shared_ptr<Type> registerSystem(Args... args) {
        std::string typeName = typeid(Type).name();
        std::shared_ptr<Type> system;

        if (mSystems.find(typeName) != mSystems.end()) {
            system = getSystem<Type>();
        } else {
            system = std::make_shared<Type>(args...);
            // Create a ComponentArray pointer and add it to the component arrays map
            mSystems.insert({typeName, system});
        }
        return system;
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
        getEntity(entityID)->types() |= getComponentArray<Type>()->get(entityID).type();
    }
    /**
     * @brief Remove a component of type Type from the entity/gameobject with entityID.
     */
    template <typename Type>
    void removeComponent(int entityID) {
        getEntity(entityID)->types() &= ~getComponentArray<Type>()->get(entityID).type();
        // Remove a component from the array for an entity
        getComponentArray<Type>()->remove(entityID);
    }
    /**
     * @brief Get a reference to the Type component owned by entityID
     */
    template <typename Type>
    Type &getComponent(int entityID) {
        CType typeMask = getEntity(entityID)->types();
        // Get a reference to a component from the array for an entity
        return getComponentArray<Type>()->get(entityID, typeMask);
    }
    template <typename Type>
    std::shared_ptr<Type> getSystem() {
        std::string typeName = typeid(Type).name();
        return std::static_pointer_cast<Type>(mSystems[typeName]);
    }
    /**
     * @brief get a reference to the last component created of that Type, if you don't have or don't need the entityID
     */
    template <typename Type>
    Type &getLastComponent() {
        // Get a reference to a component from the array for an entity
        return getComponentArray<Type>()->back();
    }
    /**
     * @brief entityDestroyed is called when an entity is removed from the game.
     * Iterates through all the Pools, and if they contain a component owned by entityID, delete and re-arrange the Pool.
     * @param entityID
     */
    void entityDestroyed(int entityID) {
        // Notify each component array that an entity has been destroyed.
        // If it has a component for that entity, it will remove it.
        if (contains(entityID, CType::Transform))
            getComponentArray<Transform>()->remove(entityID);
        if (contains(entityID, CType::Material))
            getComponentArray<Material>()->remove(entityID);
        if (contains(entityID, CType::Mesh))
            getComponentArray<Mesh>()->remove(entityID);
        if (contains(entityID, CType::Light))
            getComponentArray<Light>()->remove(entityID);
        if (contains(entityID, CType::Input))
            getComponentArray<Input>()->remove(entityID);
        if (contains(entityID, CType::Physics))
            getComponentArray<Physics>()->remove(entityID);
        if (contains(entityID, CType::Sound))
            getComponentArray<Sound>()->remove(entityID);
    }
    bool contains(GLuint eID, CType type) {
        CType typeMask = getEntity(eID)->types();
        return (typeMask & type) != CType::None;
    }
    void addBillBoard(GLuint entityID) { mBillBoards.push_back(entityID); }
    void removeBillBoardID(GLuint entityID);
    std::vector<GLuint> billBoards() { return mBillBoards; }

    GLuint makeEntity(const QString &name = "");
    std::map<GLuint, Entity *> getEntities() const { return mEntities; }
    Entity *getEntity(GLuint eID);

    void removeEntity(GLuint eID);
    GLuint numEntities() { return mEntities.size(); }

    void clearScene();

    void updateChildParent();

    void makeSnapshot();
    void loadSnapshot();
    void removeChild(const GLuint eID, const GLuint childID);
    void addChild(const GLuint eID, const GLuint childID);
    /**
     * @brief Check if Transform component (or rather its entity) has a parent object
     * @param eID
     * @return
     */
    bool hasParent(GLuint eID);
    /**
     * @brief Remove the entity's old parent (if it had one) and set it to the given parentID. If parentID == -1, simply removes the parent.
     * @param eID
     * @param parentID
     */
    void setParent(GLuint eID, int parentID, bool fromEditor = false);
    Transform &getParent(GLuint eID);
    std::vector<GLuint> getChildren(GLuint eID);

signals:
    void entityCreated(GLuint eID);
    void entityRemoved(GLuint eID);
    void parentChanged(GLuint childID);
    void poolChanged(std::shared_ptr<IPool>);

private:
    static Registry *mInstance;
    std::map<std::string, std::shared_ptr<IPool>> mPools{};
    std::map<std::string, std::shared_ptr<ISystem>> mSystems{};

    std::map<GLuint, Entity *> mEntities; // Save GameObjects as pointers to avoid clipping of derived classes
    std::vector<GLuint> mBillBoards;
    bool isBillBoard(GLuint entityID);

    std::tuple<std::map<GLuint, Entity *>, std::vector<GLuint>, std::map<std::string, std::shared_ptr<IPool>>> mSnapshot;

    // Convenience function to get the statically casted pointer to the ComponentArray of type T.
    template <typename Type>
    std::shared_ptr<Pool<Type>> getComponentArray() {
        std::string typeName = typeid(Type).name();
        return std::static_pointer_cast<Pool<Type>>(mPools[typeName]);
    }
};

#endif // REGISTRY_H
