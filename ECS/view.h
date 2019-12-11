#ifndef VIEW_H
#define VIEW_H
#include "pool.h"
#include <algorithm>
/**
 * Multi-Component View.
 * Searches for the smallest pool of components and uses this pool
 * to check if an entity contains the other types as well.
 * Less performant than groups because it doesn't modify/sort the underlying pool,
 * but for that same reason it's probably also safer/easier to use for simple tasks.
 * A run-time assert will be called if trying to get an entity that doesn't belong to the view.
 * Only Registry is allowed to create Views.
 */
template <typename... Component>
class View {
    using underlying_iterator_type = typename IPool::iterator;
    using unchecked_type = std::array<const IPool *, (sizeof...(Component) - 1)>;
    /**
     * The iterator class is a custom iterator for use with views to allow the use of for-loops
     */
    class iterator {
        friend class View<Component...>;

        iterator(unchecked_type other, underlying_iterator_type first, underlying_iterator_type last)
            : unchecked{other},
              begin{first},
              end{last}
        {
            if (begin != end && !valid()) {
                ++(*this);
            }
        }
        bool valid() const
        {
            return std::all_of(unchecked.cbegin(), unchecked.cend(), [this](const IPool *view) {
                return view->has(*begin);
            });
        }

    public:
        using difference_type = typename underlying_iterator_type::difference_type;
        using pointer = typename underlying_iterator_type::pointer;
        using reference = typename underlying_iterator_type::reference;
        using iterator_category = std::forward_iterator_tag;

        iterator() = default;

        iterator &operator++()
        {
            return (++begin != end && !valid()) ? ++(*this) : *this;
        }

        iterator operator++(int)
        {
            iterator orig = *this;
            return ++(*this), orig;
        }

        bool operator==(const iterator &other) const
        {
            return other.begin == begin;
        }

        bool operator!=(const iterator &other) const
        {
            return !(*this == other);
        }

        pointer operator->() const
        {
            return begin.operator->();
        }

        reference operator*() const
        {
            return *operator->();
        }

    private:
        unchecked_type unchecked;
        underlying_iterator_type begin;
        underlying_iterator_type end;
    };

public:
    template <typename Comp>
    /**
     * Number of entities observed by this View.
     * @return
     */
    size_t size() const
    {
        return std::get<Pool<Comp> *>(pools)->size();
    }
    /**
     * Iterator pointing to the first entity observed by the View.
     * @return
     */
    iterator begin() const
    {
        const auto view{candidate()};
        return iterator{unchecked(view), view->begin(), view->end()};
    }
    /**
     * Iterator pointing past the last entity observed by the View.
     * @return
     */
    iterator end() const
    {
        const auto view{candidate()};
        return iterator{unchecked(view), view->end(), view->end()};
    }
    template <typename Comp>
    /**
     * Returns a reference to the vector containing the given component type.
     * @tparam Type of Component.
     * @return
     */
    std::vector<Comp> &data()
    {
        return std::get<Pool<Comp>>(pools).data();
    }
    template <typename Comp>
    /**
     * Returns a reference to the vector containing the entities owning the given component type.
     * @tparam Type of Component.
     * @return
     */
    std::vector<int> &entities() const
    {
        return std::get<Pool<Comp>>(pools)->entities();
    }
    /**
     * Finds the entity list location of an entity given its ID.
     * Can return -1, in which case it is not contained in this View.
     * @param entt
     * @return
     */
    int find(const int &entt) const
    {
        return candidate()->find(entt);
    }
    /**
     * Helper function to check if an entity is contained within this View.
     * @param entt Entity ID.
     * @return
     */
    bool contains(const int &entt) const
    {
        if (entt < 0)
            return false;
        return find(entt) != -1;
    }
    /**
     * Retrieves the desired components from an entity
     * If multiple component types entered, the returned value will be a tuple of each component.
     * Retrieving these components is easiest done using structured bindings.
     * @example auto [trans, mat, mesh]{view.get<Transform, Material, Mesh>(entity)};
     * A single component type is retrieved as such:
     * @example auto &trans{view.get<Transform>(entity)};
     * @return Either a reference to one component or a tuple containing a reference to each component type.
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
    View(Pool<Component> *... ref)
        : pools{ref...}
    {
    }
    /// Pools observed by this View.
    const std::tuple<Pool<Component> *...> pools;

    /**
     * Returns the smallest pool of components for use with begin() and end() functions
     * @return
     */
    const IPool *candidate() const
    {
        return std::min({static_cast<const IPool *>(std::get<Pool<Component> *>(pools))...}, [](const auto lhs, const auto rhs) {
            return lhs->size() < rhs->size();
        });
    }
    unchecked_type unchecked(const IPool *view) const
    {
        unchecked_type other{};
        typename unchecked_type::size_type pos{};
        ((std::get<Pool<Component> *>(pools) == view ? nullptr : (other[pos++] = std::get<Pool<Component> *>(pools))), ...);
        return other;
    }
    friend class Registry;
};
/**
 * Single Component View.
 * No need to check for entity existence or anything else,
 * which makes this a useful and fast utility for accessing
 * all the components of a type.
 * Only Registry is allowed to create Views.
 */
template <typename Component>
class View<Component> {
    using iterator_type = typename IPool::iterator;

public:
    /**
     * Number of entities observed by this View.
     * @return
     */
    size_t size() const
    {
        return pool->size();
    }
    /**
     * Iterator pointing to the first entity observed by the View.
     * @return
     */
    iterator_type begin() const
    {
        return pool->begin();
    }
    /**
     * Iterator pointing past the last entity observed by the View.
     * @return
     */
    iterator_type end() const
    {
        return pool->end();
    }
    /**
     * Returns a reference to the vector containing the component type contained in this View.
     * @return
     */
    std::vector<Component> &data() const
    {
        return pool->data();
    }
    /**
     * Pointer to the first item in the entity list.
     * @return
     */
    const GLuint *entities() const
    {
        return pool->entities();
    }
    /**
     * Finds an entity's location in the Sparse Set.
     * @param entt
     * @return
     */
    int find(const int &entt) const
    {
        return pool->find(entt);
    }
    /**
     * Checks if an entity is contained in this View.
     * @param entt
     * @return
     */
    bool contains(const int &entt) const
    {
        return find(entt) != -1;
    }
    /**
     * @brief Checks whether the view is empty.
     * @return True if the view is empty, false otherwise.
     */
    bool empty() const
    {
        return pool->empty();
    }
    /**
    * Same as the multi-component view, but here you don't need to enter a component type since it's implicitly discovered
    */
    decltype(auto) get(const int &entt) const
    {
        assert(contains(entt));
        return pool->get(entt);
    }

private:
    View(Pool<Component> *ref)
        : pool{ref} {}
    /// Pool observed by this View.
    Pool<Component> *pool;
    friend class Registry;
};

#endif // VIEW_H
