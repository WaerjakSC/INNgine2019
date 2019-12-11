#ifndef GROUP_H
#define GROUP_H
#include "pool.h"

/**
 * Fully owned group.
 * The first template type will be owned by the group, allowing the group to freely sort that type's Pool to place all the group's members in front.
 */
template <typename... Owned>
class Group {
public:
    using iterator = typename IPool::iterator;
    template <typename Comp>
    /**
     * Number of entities managed by the group.
     * Size of the underlying entity list.
     * @return
     */
    size_t size() const
    {
        return std::get<Pool<Comp> *>(pools)->size();
    }
    template <typename Comp>
    /**
     * Reference to the vector containing all the components used by the group.
     * May contain more than just the entities in the group, use in combination with size() to ensure safe usage.
     * @return
     */
    std::vector<Comp> &data()
    {
        return std::get<Pool<Comp>>(pools).data();
    }
    template <typename Comp>
    /**
     * Direct access to the list of entities. Guaranteed to be tightly packed for fast iterating.
     * @return
     */
    const GLuint *entities() const
    {
        return std::get<Pool<Comp>>(pools)->entities();
    }
    /**
     * Find an entity's position in the dense part of the Sparse Set.
     * @param entityID
     * @return
     */
    int find(const GLuint entityID) const
    {
        return std::get<0>(pools)->find(entityID);
    }
    /**
     * Checks if a given entity is contained within the group.
     * @param entt
     * @return
     */
    bool contains(const int &entt) const
    {
        if (entt < 0)
            return false;
        return find(entt) != -1;
    }
    /**
     * Iterator pointing to the first entity in the group for use with range-based for loops.
     * @return
     */
    iterator begin() const
    {
        return std::get<0>(pools)->end() - *length;
    }
    /**
     * Iterator pointing past the last entity in the group, for use with range-based for loops.
     * @return
     */
    iterator end() const
    {
        return std::get<0>(pools)->end();
    }
    /**
     * Checks whether the view is empty.
     * @return True if the view is empty, false otherwise.
     */
    bool empty() const
    {
        return std::get<0>(pools)->empty();
    }
    /**
     * Retrieves the desired components from an entity.
     * If multiple component types entered, the returned value will be a tuple of each component.
     * Retrieving these components is easiest done using structured bindings.
     * @example auto [trans, mat, mesh] = view.get<Transform, Material, Mesh>(entity);
     */
    template <typename... Comp>
    decltype(auto) get(const int &entt) const
    {
        assert(contains(entt));
        if constexpr (sizeof...(Comp) == 1) {
            return (std::get<Pool<Comp> *>(pools)->get(entt), ...);
        }
        else
            return std::tuple<decltype(get<Comp>(entt))...>{get<Comp>(entt)...};
    }

private:
    Group(const size_t *extent, Pool<Owned> *... owned) : pools{owned...}, length(extent)
    {
    }
    /// Owned pools
    const std::tuple<Pool<Owned> *...> pools;
    /// Number of components owned by this Group.
    const size_t *length;

    friend class Registry;
};

#endif // GROUP_H
