#ifndef REGISTRY_H
#define REGISTRY_H

#include "group.h"
#include "isystem.h"
#include "pool.h"
#include "view.h"

#include "resourcemanager.h"
#include <memory>
#include <typeinfo>
#include <unordered_set>
struct GroupData {
    size_t extent;
    std::unordered_set<std::string> ownedPools;
    template <typename T>
    bool contains() {
        std::string type = typeid(T).name();
        if (ownedPools.find(type) == ownedPools.end())
            return false;
        else
            return true;
    }
};

class Registry : public QObject {
    Q_OBJECT
public:
    Registry();
    static Registry *instance();
    virtual ~Registry() {}

    /**
     * @brief Creates a view guaranteed to contain the entities containing all the components given in the template argument.
     * Essentially free to create, lifetime should be very low to ensure a view is up to date.
     * Simply create to get the entities you want, and discard immediately after use.
     * @tparam List of the component types you want the view to contain.
     */
    template <typename... Comp>
    inline View<Comp...> view() {
        return View{getPool<Comp>()...};
    }
    template <typename... Owned>
    Group<Owned...> group() {
        const auto cpools = std::make_tuple(getPool<Owned>()...);
        const size_t extent[1]{sizeof...(Owned)};
        if (auto it = std::find_if(mGroupData.cbegin(), mGroupData.cend(), [&extent](const GroupData &groupData) {
                return std::equal(std::begin(extent), std::end(extent), groupData.extent) && (groupData.contains<Owned>() && ...);
            });
            it != mGroupData.cend()) {
        }
    }
    /**
     * @brief Register component type. A component must be registered before it can be used by the ECS.
     */
    template <typename Type>
    inline void registerComponent() {
        std::string typeName{typeid(Type).name()};

        if (mPools.find(typeName) != mPools.end()) {
            return;
        } else {
            // Create a ComponentArray pointer and add it to the component arrays map
            mPools.emplace(typeName, std::make_unique<Pool<Type>>());
        }
    }
    /**
     * @brief Register a system with the ECS. Not strictly required, but will allow you to use the registry to get a reference to the system.
     */
    template <typename Type, class... Args>
    inline Ref<Type> registerSystem(Args... args) {
        std::string typeName{typeid(Type).name()};

        if (mSystems.find(typeName) != mSystems.end() && mSystems[typeName]) {
            return system<Type>();
        } else {
            // Create a ComponentArray pointer and add it to the component arrays map
            Ref<Type> newSystem = std::make_shared<Type>(args...);
            mSystems.insert({typeName, newSystem});
            return newSystem;
        }
    }
    /**
    * @brief Make a generic entity with no components attached.
    * @param name Name of the entity. Leave blank if no name desired.
    * @return Returns the entity ID for use in adding components or other tasks.
    */
    template <typename... Component>
    GLuint makeEntity(const QString &name = "", bool signal = true) {
        GLuint eID{nextAvailable()};
        if (contains<EInfo>(eID)) {
            newGeneration(eID, name);
        } else
            add<EInfo>(eID, name);
        assert((typeid(Component) != typeid(EInfo)) && ...);
        (add<Component>(eID), ...);
        if (signal)
            emit entityCreated(eID);
        return eID;
    }
    /**
     * @brief Adds an entity and its new component to a pool of that type.
     * Entity is equivalent to Component in this case, since a pool won't contain the entity if the entity doesn't have the component.
     * @example the Transform component type can take 3 extra variables, add<Transform>(entityID, position, rotation, scale) will initialize its variables to custom values.
     * @param entityID
     * @tparam Args... args Variadic parameter pack - Use as many function parameters as needed to construct the component.
     */
    template <typename Type, typename... Args>
    inline void add(GLuint entityID, Args... args) {
        // Add a component to the array for an entity
        getPool<Type>()->add(entityID, args...);
    }
    /**
     * @brief Remove a component of type Type from the entity with entityID.
     */
    template <typename Type>
    inline void remove(GLuint entityID) {
        // Remove a component from the array for an entity
        getPool<Type>()->remove(entityID);
    }
    /**
     * @brief Get a reference to the Type component owned by entityID
     */
    template <typename Type>
    inline Type &get(GLuint entityID) {
        // Get a reference to a component from the array for an entity
        return getPool<Type>()->get(entityID);
    }
    template <typename... Type>
    inline decltype(auto) system() {
        if constexpr (sizeof...(Type) == 1) {
            return (getSystem<Type>(), ...);
        } else
            return std::tuple<decltype(getSystem<Type>())...>{getSystem<Type>()...};
    }
    /**
     * @brief get a reference to the last component created of that Type, if you don't have or don't need the entityID
     */
    template <typename Type>
    inline Type &getLastComponent() {
        // Get a reference to a component from the array for an entity
        return getPool<Type>()->back();
    }
    /**
     * @brief entityDestroyed is called when an entity is removed from the game.
     * Iterates through all the Pools, and if they contain a component owned by entityID, delete and re-arrange the Pool.
     * @param entityID
     */
    inline void entityDestroyed(GLuint entityID) {
        // Notify each component array that an entity has been destroyed.
        // If it has a component for that entity, it will remove it.
        for (auto &pool : mPools) {
            if (pool.first == typeid(EInfo).name())
                continue;
            if (pool.second->has(entityID)) {
                pool.second->remove(entityID);
            }
        }
    }
    /**
     * @brief Checks if an entity owns the given component types.
     */
    template <typename... Type>
    inline bool contains(GLuint eID) {
        [[maybe_unused]] const auto cpools{std::make_tuple(getPool<Type>()...)};
        return ((std::get<Pool<Type> *>(cpools) ? std::get<Pool<Type> *>(cpools)->has(eID) : false) && ...);
    }
    /**
     * @brief Simple getter for a type's typeid name
     */
    template <typename Type>
    inline std::string type() {
        return typeid(Type).name();
    }

    /**
     * @brief duplicateEntity Creates an entity with the exact same name and components as the duped entity.
     * @param dupedEntity
     * @return
     */
    GLuint duplicateEntity(GLuint dupedEntity);
    inline const std::vector<GLuint> &getEntities() { return getPool<EInfo>()->entities(); }
    /**
    * @brief Get a pointer to the entity with the specified ID.
    * @param eID
    * @return
    */
    EInfo &getEntity(GLuint eID);
    /**
    * @brief Destroy an entity - Clears all components from an entity ID and readies that ID for use with the next created entity
    * @param eID - entityID
    */
    void removeEntity(GLuint eID);
    /**
     * @brief numEntities size of the mEntities map
     * @return
     */
    inline GLuint numEntities() {
        std::vector<GLuint> entities{getPool<EInfo>()->entities()};
        return entities.size();
    }
    /**
     * @brief nextAvailable get the next available ID. In a scenario with no destroyed IDs, this will simply return the size of the mEntities map.
     * Destroyed entities give up their ID for use with a new entity. This avoids unnecessary bloating.
     * @return
     */
    GLuint nextAvailable();
    /**
     * @brief clearScene Simply calls removeEntity() on every entity in the scene.
     */
    void clearScene();
    /**
    * @brief Updates the child parent hierarchy in case it's out of date
    */
    void updateChildParent();
    /**
     * @brief makeSnapshot takes a snapshot of the current entities etc.
     */
    void makeSnapshot();
    /**
     * @brief loadSnapshot loads the snapshot taken by makeSnapshot()
     */
    void loadSnapshot();
    /**
     * @brief removeChild remove a child from an entity
     * @param eID
     * @param childID
     */
    void removeChild(const GLuint eID, const GLuint childID);
    /**
     * @brief addChild Make an entity the child of another entity.
     * @param eID
     * @param childID
     */
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
    /**
     * @brief getParent get the parent of the given entity. Undefined behaviour of the entity has no parent.
     * @param eID
     * @return
     */
    Transform &getParent(GLuint eID);
    /**
     * @brief getChildren get a list of an entity's children.
     * @param eID
     * @return
     */
    std::vector<GLuint> getChildren(GLuint eID);

    GLuint getSelectedEntity() const;
    bool isDestroyed(GLuint entityID);
public slots:
    void setSelectedEntity(const GLuint selectedEntity);

signals:
    void entityCreated(GLuint eID);
    void entityRemoved(GLuint eID);
    void parentChanged(GLuint childID);
    void poolChanged(IPool *pool);
    void nameChanged(GLuint eID);

private:
    static Registry *mInstance;
    std::map<std::string, Scope<IPool>> mPools{};
    std::map<std::string, Ref<ISystem>> mSystems{};
    std::vector<GroupData> mGroupData{};
    std::vector<GLuint> mAvailableSlots;
    void newGeneration(GLuint id, const QString &text);

    GLuint mSelectedEntity;
    std::map<std::string, IPool *> mSnapshot;

    template <typename Type>
    inline Ref<Type> getSystem() {
        std::string typeName{type<Type>()};
        return std::static_pointer_cast<Type>(mSystems[typeName]);
    }
    // Convenience function to get the statically casted pointer to the Pool of type Type.
    template <typename Type>
    inline Pool<Type> *getPool() {
        std::string typeName{type<Type>()};
        return static_cast<Pool<Type> *>(mPools[typeName].get());
    }
};

#endif // REGISTRY_H
