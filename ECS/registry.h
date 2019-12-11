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
struct GroupHandler {
    GroupHandler(const std::tuple<Pool<Member> *...> pools)
        : ownedPools(pools) {}
    const std::tuple<Pool<Member> *...> ownedPools;
    size_t owned{};
};
struct GroupData {
    template <typename Func>
    GroupData(size_t ext, const Func &ownedType)
        : extent(ext), ownsType(ownedType) {}
    size_t extent;
    bool (*ownsType)(const std::string &);
    std::unordered_set<std::string> pools;
    size_t owned{};
};
class Registry : public QObject {
    Q_OBJECT
    /**
     * @brief Simple getter for a type's typeid name
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
     * @brief Creates a view guaranteed to contain the entities containing all the components given in the template argument.
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
     * @brief Register component type. A component must be registered before it can be used by the ECS.
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
     * @brief Register a system with the ECS. Not strictly required, but will allow you to use the registry to get a reference to the system.
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
    * @brief Make a generic entity.
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
     * @brief Adds an entity and its new component to a pool of that type.
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
    void onConstruct(const std::string &type, const GLuint entityID);
    template <typename Type>
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
    /**
     * @brief Remove a component of type Type from the entity with entityID.
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
     * @brief Get a reference to the Type component owned by entityID
     */
    template <typename Type>
    Type &get(GLuint entityID)
    {
        // Get a reference to a component from the array for an entity
        return getPool<Type>()->get(entityID);
    }
    template <typename... Type>
    decltype(auto) system()
    {
        if constexpr (sizeof...(Type) == 1) {
            return (getSystem<Type>(), ...);
        }
        else
            return std::tuple<decltype(getSystem<Type>())...>{getSystem<Type>()...};
    }
    /**
     * @brief get a reference to the last component created of that Type, if you don't have or don't need the entityID
     */
    template <typename Type>
    Type &getLastComponent()
    {
        // Get a reference to a component from the array for an entity
        return getPool<Type>()->back();
    }
    /**
     * @brief entityDestroyed is called when an entity is removed from the game.
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
     * @brief Checks if an entity owns the given component types.
     */
    template <typename... Type>
    bool contains(GLuint eID)
    {
        [[maybe_unused]] const auto cpools{std::make_tuple(getPool<Type>()...)};
        return ((std::get<Pool<Type> *>(cpools) ? std::get<Pool<Type> *>(cpools)->has(eID) : false) && ...);
    }

    /**
     * @brief duplicateEntity Creates an entity with the exact same name and components as the duped entity.
     * @param dupedEntity
     * @return
     */
    GLuint duplicateEntity(GLuint dupedEntity);
    const std::vector<GLuint> getEntities() { return getPool<EInfo>()->entityList(); }
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
    GLuint numEntities()
    {
        return getPool<EInfo>()->entityList().size();
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
    std::map<std::string, cjk::Scope<IPool>> mPools{};
    std::map<std::string, cjk::Ref<ISystem>> mSystems{};
    std::vector<uint> mAvailableIDs;
    std::vector<GroupData *> mGroups{};
    void newGeneration(GLuint id, const QString &text);

    GLuint mSelectedEntity{0};
    std::tuple<std::vector<GLuint>, std::vector<GroupData *>, std::map<std::string, IPool *>> mSnapshot;

    template <typename Type>
    cjk::Ref<Type> getSystem()
    {
        std::string typeName{type<Type>()};
        return std::static_pointer_cast<Type>(mSystems[typeName]);
    }
    IPool *getPool(const std::string &type)
    {
        return mPools[type].get();
    }
    // Convenience function to get the statically casted pointer to the Pool of type Type.
    template <typename Type>
    Pool<Type> *getPool()
    {
        std::string typeName{type<Type>()};
        return static_cast<Pool<Type> *>(mPools[typeName].get());
    }
};

#endif // REGISTRY_H
