#ifndef REGISTRY_H
#define REGISTRY_H

#include "entity.h"
#include "isystem.h"
#include "pool.h"
#include "view.h"

#include "resourcemanager.h"
#include <memory>
#include <typeinfo>

class Registry : public QObject {
    Q_OBJECT
public:
    Registry();
    static Registry *instance();
    virtual ~Registry() {}

    template <typename... Comp>
    View<Comp...> view() {
        return {getPool<Comp>()...};
    }

    /**
     * Register component type. For systems that own the component type
     */
    template <typename Type>
    void registerComponent(Pool<Type> *pool) {
        std::string typeName = typeid(Type).name();

        // Create a ComponentArray pointer and add it to the component arrays map
        mPools.insert({typeName, pool});
    }
    /**
     * For partially owned component types, systems only want a shared ptr to the pool
     * Will register the component type if it exists, otherwise just returns the pool type
     */
    template <typename Type>
    Pool<Type> *registerComponent() {
        std::string typeName = typeid(Type).name();
        Pool<Type> *pool;

        if (mPools.find(typeName) != mPools.end()) {
            pool = getPool<Type>();
        } else {
            // Create a ComponentArray pointer and add it to the component arrays map
            pool = new Pool<Type>;
            mPools.insert({typeName, pool});
        }
        return pool;
    }
    template <typename Type, class... Args>
    Type *registerSystem(Args... args) {
        std::string typeName = typeid(Type).name();
        Type *system;

        if (mSystems.find(typeName) != mSystems.end() && mSystems[typeName]) {
            system = getSystem<Type>();
        } else {
            // Create a ComponentArray pointer and add it to the component arrays map
            system = new Type(args...);
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
        getPool<Type>()->add(entityID, args...);
        getEntity(entityID)->types() |= getPool<Type>()->get(entityID).type();
    }
    /**
     * @brief Remove a component of type Type from the entity/gameobject with entityID.
     */
    template <typename Type>
    void removeComponent(int entityID) {
        getEntity(entityID)->types() &= ~getPool<Type>()->get(entityID).type();
        // Remove a component from the array for an entity
        getPool<Type>()->remove(entityID);
    }
    /**
     * @brief Get a reference to the Type component owned by entityID
     */
    template <typename Type>
    Type &getComponent(int entityID) {
        // Get a reference to a component from the array for an entity
        return getPool<Type>()->get(entityID);
    }
    template <typename Type>
    Type *getSystem() {
        std::string typeName = typeid(Type).name();
        return static_cast<Type *>(mSystems[typeName]);
    }
    /**
     * @brief get a reference to the last component created of that Type, if you don't have or don't need the entityID
     */
    template <typename Type>
    Type &getLastComponent() {
        // Get a reference to a component from the array for an entity
        return getPool<Type>()->back();
    }
    /**
     * @brief entityDestroyed is called when an entity is removed from the game.
     * Iterates through all the Pools, and if they contain a component owned by entityID, delete and re-arrange the Pool.
     * @param entityID
     */
    void entityDestroyed(int entityID) {
        // Notify each component array that an entity has been destroyed.
        // If it has a component for that entity, it will remove it.
        for (auto &pool : mPools) {
            if (pool.second->has(entityID)) {
                pool.second->remove(entityID);
            }
        }
    }
    template <typename Type>
    bool contains(GLuint eID) {
        return getPool<Type>()->has(eID);
    }
    void addBillBoard(GLuint entityID) { mBillBoards.push_back(entityID); }
    void removeBillBoardID(GLuint entityID);
    std::vector<GLuint> billBoards() { return mBillBoards; }

    GLuint makeEntity(const QString &name = "", bool signal = true);
    GLuint duplicateEntity(GLuint dupedEntity);
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
    void poolChanged(IPool *pool);

private:
    static Registry *mInstance;
    std::map<std::string, IPool *> mPools{};
    std::map<std::string, ISystem *> mSystems{};

    std::map<GLuint, Entity *> mEntities; // Save GameObjects as pointers to avoid clipping of derived classes
    std::vector<GLuint> mAvailableSlots;
    std::vector<GLuint> mBillBoards;
    bool isBillBoard(GLuint entityID);

    std::tuple<std::map<GLuint, Entity *>, std::vector<GLuint>, std::map<std::string, IPool *>> mSnapshot;

    // Convenience function to get the statically casted pointer to the ComponentArray of type T.
    template <typename Type>
    Pool<Type> *getPool() {
        std::string typeName = typeid(Type).name();
        return static_cast<Pool<Type> *>(mPools[typeName]);
    }
};

#endif // REGISTRY_H
