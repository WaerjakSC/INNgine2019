#ifndef GROUP_H
#define GROUP_H
#include "pool.h"

/**
 * @brief Fully owned group.
 * The first template type will be owned by the group, allowing the group to freely sort that type's Pool according to
 */
template <typename... Owned>
class Group {
public:
    using iterator = typename IPool::iterator;
    template <typename Comp>
    size_t size() const {
        return std::get<Pool<Comp> *>(pools)->size();
    }
    template <typename Comp>
    std::vector<Comp> &data() {
        return std::get<Pool<Comp>>(pools).data();
    }
    template <typename Comp>
    const GLuint *entities() const {
        return std::get<Pool<Comp>>(pools)->entities();
    }
    int find(const int &entt) const {
        return std::get<0>(pools)->find(entt);
    }
    bool contains(const int &entt) const {
        if (entt < 0)
            return false;
        return find(entt) != -1;
    }
    iterator begin() const {
        return std::get<0>(pools)->end() - *length;
    }
    iterator end() const {
        return std::get<0>(pools)->end();
    }
    /**
     * @brief Checks whether the view is empty.
     * @return True if the view is empty, false otherwise.
     */
    bool empty() const {
        return std::get<0>(pools)->empty();
    }
    /**
     * @brief Retrieves the desired components from an entity
     * If multiple component types entered, the returned value will be a tuple of each component.
     * Retrieving these components is easiest done using structured bindings.
     * @example auto [trans, mat, mesh] = view.get<Transform, Material, Mesh>(entity);
     */
    template <typename... Comp>
    decltype(auto) get(const int &entt) const {
        assert(contains(entt));
        if constexpr (sizeof...(Comp) == 1) {
            return (std::get<Pool<Comp> *>(pools)->get(entt), ...);
        } else
            return std::tuple<decltype(get<Comp>(entt))...>{get<Comp>(entt)...};
    }

private:
    Group(const size_t *extent, Pool<Owned> *... owned) : pools{owned...}, length(extent) {
    }
    const size_t *length;
    const std::tuple<Pool<Owned> *...> pools;
    friend class Registry;
};

#endif // GROUP_H
