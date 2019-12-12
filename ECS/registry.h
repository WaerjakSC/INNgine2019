#ifndef REGISTRY_H
#define REGISTRY_H

#include "components.h"
#include "group.h"
#include "isystem.h"
#include "pool.h"
#include "view.h"

#include <functional>
#include <typeinfo>
#include <unordered_set>

template <typename... Member>
/**
 * The GroupHandler struct contains the pools iterated on by a Group.
 */
struct GroupHandler {
    GroupHandler(const std::tuple<Pool<Member> *...> pools)
        : ownedPools(pools) {}
    const std::tuple<Pool<Member> *...> ownedPools;
    size_t owned{};
};
/**
 * The GroupData struct is used to check if a group already exists.
 */
struct GroupData {
    template <typename Func>
    GroupData(size_t ext, const Func &ownedType)
        : extent(ext), ownsType(ownedType) {}
    size_t extent;
    /**
     * Function object for checking if a pool is already owned by a Group.
     * Checks typeid().name() against already owned pools' typenames.
     */
    bool (*ownsType)(const std::string &);
    std::unordered_set<std::string> pools;
    size_t owned{};
};
class Registry : public QObject {
    Q_OBJECT
    /**
     * Simple getter for a type's typeid name.
     */
    template <typename Type>
    static std::string type()
    {
        return typeid(Type).name();
    }

public:
    Registry();
    static Registry *instance();
    virtual ~Registry() {}

    /**
     * Creates a view guaranteed to contain the entities containing all the components given in the template argument.
     * Essentially free to create, lifetime should be very low to ensure a view is up to date.
     * Simply create to get the entities you want, and discard immediately after use.
     * @tparam List of the component types you want the view to contain.
     */
    template <typename... Comp>
    View<Comp...> view()
    {
        return View{getPool<Comp>()...};
    }

    template <typename... Owned>
    /**
     * Creates a group guaranteed to contain the entities that own all the components given in the template argument.
     * Essentially free to create, can and should be discarded every frame.
     * First time initialization can be somewhat expensive if done while entities exist. Create before entities are created to have the group sort them as they are created.
     * Slight performance cost to component creation in the Pools owned by a Group, however this is more than offset by the gain in performance from being able to guarantee
     * that all the components iterated by the Group will be tightly packed in memory.
     * @return
     */
    Group<Owned...> group()
    {
        const auto cpools{std::make_tuple(getPool<Owned>()...)};
        const size_t extent{sizeof...(Owned)};
        GroupData *data{nullptr};
        if (auto it = std::find_if(mGroups.cbegin(), mGroups.cend(), [&extent](const auto groupData) {
                return extent == groupData->extent && (groupData->ownsType(type<Owned>()) && ...);
            });
            it != mGroups.cend()) {
            data = *it;
        }
        if (!data) {
            data = new GroupData{sizeof...(Owned),
                                 [](const std::string &ctype) { return ((ctype == type<Owned>()) || ...); }};
            (data->pools.insert(type<Owned>()), ...);
            mGroups.emplace_back(std::move(data));
            // find the smallest pool to use as the sorting "master"
            const auto *cpool = std::min({static_cast<const IPool *>(std::get<Pool<Owned> *>(cpools))...}, [](const auto lhs, const auto rhs) {
                return lhs->size() < rhs->size();
            });
            // Do the initial swapping to prepare the group -- basically free if done before any entities exist
            std::for_each(cpool->entities(), cpool->entities() + cpool->size(), [cpools, data](const GLuint entity) {
                if ((std::get<Pool<Owned> *>(cpools)->has(entity) && ...)) {
                    if (!(std::get<0>(cpools)->index(entity) < data->owned)) {
                        const auto pos = data->owned++;
                        (std::get<Pool<Owned> *>(cpools)->swap(std::get<Pool<Owned> *>(cpools)->entities()[pos], entity), ...);
                    }
                }
            });
        }
        return {&data->owned, std::get<Pool<Owned> *>(cpools)...};
    }
    /**
     * Register component type. A component must be registered before it can be used by the ECS.
     */
    template <typename Type>
    void registerComponent()
    {
        std::string typeName{typeid(Type).name()};

        if (mPools.find(typeName) != mPools.end()) {
            return;
        }
        else {
            // Create a ComponentArray pointer and add it to the component arrays map
            mPools.emplace(typeName, std::make_unique<Pool<Type>>());
        }
    }
    /**
     * Register a system with the ECS. Not strictly required, but will allow you to use the registry to get a reference to the system.
     */
    template <typename Type, class... Args>
    cjk::Ref<Type> registerSystem(Args... args)
    {
        std::string typeName{typeid(Type).name()};

        if (mSystems.find(typeName) != mSystems.end() && mSystems[typeName]) {
            return system<Type>();
        }
        else {
            // Create a ComponentArray pointer and add it to the component arrays map
            cjk::Ref<Type> newSystem = std::make_shared<Type>(args...);
            mSystems.insert({typeName, newSystem});
            return newSystem;
        }
    }
    /**
    * Make a generic entity.
    * Add default state components by writing makeEntity<ComponentType1, ComponentType2, etc...>(name, signal)
    * @param name Name of the entity. Leave blank if no name desired.
    * @return Returns the entity ID for use in adding components or other tasks.
    */
    template <typename... Component>
    GLuint makeEntity(const QString &name = "", bool signal = true)
    {
        GLuint eID{nextAvailable()};
        if (contains<EInfo>(eID)) {
            newGeneration(eID, name);
        }
        else
            add<EInfo>(eID, name);
        if (signal)
            emit entityCreated(eID);
        if constexpr (sizeof...(Component) > 0)
            (add<Component>(eID), ...);
        return eID;
    }
    /**
     * Adds an entity and its new component to a pool of that type.
     * Entity is equivalent to Component in this case, since a pool won't contain the entity if the entity doesn't have the component.
     * @example the Transform component type can take 3 extra variables, add<Transform>(entityID, position, rotation, scale) will initialize its variables to custom values.
     * @param entityID
     * @tparam Args... args Variadic parameter pack - Use as many function parameters as needed to construct the component.
     * @return the new component in case we want to change something not in the constructor.
     */
    template <typename Type, typename... Args>
    Type &add(const GLuint entityID, Args... args)
    {
        // Add a component to the array for an entity
        Type &component{getPool<Type>()->add(entityID, args...)};

        onConstruct<Type>(entityID);
        return component;
    }
    template <typename Type>
    /**
     * Called when an entity is assigned a component owned by a Group.
     * The Group holds an integer pointing to the last member owned by the Group to enable it to sort its owned pools efficiently.
     * This function increments that integer.
     * @param entityID
     */
    void onConstruct(const GLuint entityID)
    {
        for (auto &group : mGroups) {
            if (group->ownsType(type<Type>())) {
                std::vector<IPool *> pools;
                size_t num{};
                for (auto &poolName : group->pools) {
                    auto member = getPool(poolName);
                    pools.push_back(member);
                    if (member->has(entityID) && !(static_cast<size_t>(member->index(entityID)) < group->owned)) {
                        num++;
                    }
                }
                if (num == pools.size()) {
                    const auto pos = group->owned++;
                    for (auto &pool : pools) {
                        pool->swap(pool->entities()[pos], entityID);
                    }
                }
                break;
            }
        }
    }
    void onConstruct(const std::string &type, const GLuint entityID);

    /**
     * Remove a component of type Type from the entity with entityID.
     */
    template <typename Type>
    void remove(GLuint entityID)
    {
        onDestroy<Type>(entityID);

        // Remove a component from the array for an entity
        getPool<Type>()->remove(entityID);
    }
    void onDestroy(const std::string &type, const GLuint entityID);
    template <typename Type>
    /**
     * Called when an entity is destroyed.
     * If it's owned by a Group, this will decrement the integer pointing to the last member owned by the Group,
     * and swap the entity being destroyed with the entity past the index of that integer.
     * @param entityID
     */
    void onDestroy(const GLuint entityID)
    {
        for (auto &group : mGroups) {
            if (group->ownsType(type<Type>())) {
                auto member = getPool<Type>();
                if (member->has(entityID) && static_cast<size_t>(member->index(entityID)) < group->owned) {
                    group->owned--;
                }
                for (auto &poolName : group->pools) {
                    auto member = getPool(poolName);
                    if (member->has(entityID) && static_cast<size_t>(member->index(entityID)) < group->owned) {
                        const auto pos = group->owned;
                        member->swap(member->entities()[pos], entityID);
                    }
                }
                break;
            }
        }
    }
    /**
     * Get a reference to the Type component owned by entityID
     */
    template <typename Type>
    Type &get(GLuint entityID)
    {
        // Get a reference to a component from the array for an entity
        return getPool<Type>()->get(entityID);
    }
    template <typename... Type>
    /**
    * Get a reference to one or more systems.
    * @example auto inputSys{registry->system<InputSystem>()};
    * @example auto [inputSys, moveSys]{registry->system<InputSystem, MovementSystem>()};
    */
    decltype(auto) system()
    {
        if constexpr (sizeof...(Type) == 1) {
            return (getSystem<Type>(), ...);
        }
        else
            return std::tuple<decltype(getSystem<Type>())...>{getSystem<Type>()...};
    }
    /**
     * Get a reference to the last component in the Pool of that Type.
     * Not guaranteed to be the last created component, due to how Groups may sort their pools.
     */
    template <typename Type>
    Type &getLastComponent()
    {
        // Get a reference to a component from the array for an entity
        return getPool<Type>()->back();
    }
    /**
     * Called when an entity is removed from the game.
     * Iterates through all the Pools, and if they contain a component owned by entityID, delete and re-arrange the Pool.
     * @param entityID
     */
    void entityDestroyed(GLuint entityID)
    {
        // Notify each component array that an entity has been destroyed.
        // If it has a component for that entity, it will remove it.
        for (auto &pool : mPools) {
            onDestroy(pool.first, entityID);
        }
        for (auto &pool : mPools) {
            if (pool.first == typeid(EInfo).name()) {
                mAvailableIDs.push_back(entityID);
                continue;
            }
            if (pool.second->has(entityID)) {
                pool.second->remove(entityID);
            }
        }
    }
    /**
     * Checks if an entity owns the given component types.
     */
    template <typename... Type>
    bool contains(GLuint eID)
    {
        [[maybe_unused]] const auto cpools{std::make_tuple(getPool<Type>()...)};
        return ((std::get<Pool<Type> *>(cpools) ? std::get<Pool<Type> *>(cpools)->has(eID) : false) && ...);
    }

    /**
     * Creates an entity with the exact same name and components as the duped entity.
     * @param dupedEntity
     * @return
     */
    GLuint duplicateEntity(GLuint dupedEntity);
    /**
     * Returns a list of all the entities in existence.
     * All entities are guaranteed to have the EInfo component.
     * @return
     */
    const std::vector<GLuint> getEntities() { return getPool<EInfo>()->entityList(); }
    /**
    * Get a pointer to the entity with the specified ID.
    * @param eID
    * @return
    */
    EInfo &getEntity(GLuint eID);
    /**
    * Destroy an entity.
    * Clears all components from an entity ID and readies that ID for use with the next created entity
    * @param eID - entityID
    */
    void removeEntity(GLuint eID);
    /**
     * Total number of entities in existence.
     * @return
     */
    GLuint numEntities()
    {
        return getPool<EInfo>()->entityList().size();
    }
    /**
     * Get the next available ID.
     * In a scenario with no destroyed IDs, this will simply return the number of entities + 1.
     * Destroyed entities give up their ID for use with a new entity. This avoids unnecessary bloating.
     * @return
     */
    GLuint nextAvailable();
    /**
     * Simply calls removeEntity() on every entity in the scene.
     */
    void clearScene();
    /**
    * Updates the child parent hierarchy in case it's out of date.
    */
    void updateChildParent();
    /**
     * Takes a snapshot of the current entities etc.
     */
    void makeSnapshot();
    /**
     * Loads the snapshot taken by makeSnapshot().
     */
    void loadSnapshot();
    /**
     * Remove a child from an entity.
     * @param eID
     * @param childID
     */
    void removeChild(const GLuint eID, const GLuint childID);
    /**
     * Make an entity the child of another entity.
     * @param eID
     * @param childID
     */
    void addChild(const GLuint eID, const GLuint childID);
    /**
     * Check if Transform component (or rather its entity) has a parent object.
     * @param eID
     * @return
     */
    bool hasParent(GLuint eID);
    /**
     * Remove the entity's old parent (if it had one) and set it to the given parentID. If parentID == -1, simply removes the parent.
     * @param eID
     * @param parentID
     */
    void setParent(GLuint eID, int parentID, bool fromEditor = false);
    /**
     * Get the parent of the given entity.
     * Undefined behaviour of the entity has no parent.
     * @param eID
     * @return
     */
    Transform &getParent(GLuint eID);
    /**
     * Get a list of an entity's children.
     * @param eID
     * @return
     */
    std::vector<GLuint> getChildren(GLuint eID);

    /**
     * Get the entity selected in the GUI.
     * @return
     */
    GLuint getSelectedEntity() const;
    /**
     * Check if an entity is destroyed and its ID waiting to be reassigned.
     * @param entityID
     * @return
     */
    bool isDestroyed(GLuint entityID);

    PlayerComponent &getPlayer();

public slots:
    /**
     * Sets the selected entity, usually called by MainWindow or HierarchyView.
     * @param selectedEntity
     */
    void setSelectedEntity(const GLuint selectedEntity);

signals:
    void entityCreated(GLuint eID);
    void entityRemoved(GLuint eID);
    void parentChanged(GLuint childID);
    void poolChanged(IPool *pool);
    void nameChanged(GLuint eID);

private:
    static Registry *mInstance;
    /// Contains the Pools for every registered component type.
    std::map<std::string, cjk::Scope<IPool>> mPools{};
    /// Contains every registered system.
    std::map<std::string, cjk::Ref<ISystem>> mSystems{};
    /// List of all the entityIDs waiting to be reassigned.
    std::vector<uint> mAvailableIDs;
    /**
     * Give an EInfo component to a new entity
     * @param id
     * @param text
     */
    void newGeneration(GLuint id, const QString &text);
    /// List of every existing Group.
    std::vector<GroupData *> mGroups{};
    /// Current entity selected in the GUI.
    GLuint mSelectedEntity{0};
    PlayerComponent player;
    ParticleEmitter particleEmitter;
    /// Snapshot containing relevant data for use when hitting Play/Stop in the editor.
    std::tuple<std::vector<GLuint>, std::vector<GroupData *>, std::map<std::string, IPool *>> mSnapshot;

    template <typename Type>
    /**
     * Casts the system to the correct type according to its typename.
     * @return
     */
    cjk::Ref<Type> getSystem()
    {
        std::string typeName{type<Type>()};
        return std::static_pointer_cast<Type>(mSystems[typeName]);
    }
    /**
     * Return the Pool with the given typename.
     * @param type
     * @return
     */
    IPool *getPool(const std::string &type)
    {
        return mPools[type].get();
    }
    // Convenience function to get the statically casted pointer to the Pool of type Type.
    template <typename Type>
    /**
     * Casts a Pool to the correct type according to its typename.
     * @return
     */
    Pool<Type> *getPool()
    {
        std::string typeName{type<Type>()};
        return static_cast<Pool<Type> *>(mPools[typeName].get());
    }
};

#endif // REGISTRY_H
