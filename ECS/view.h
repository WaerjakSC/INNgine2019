#ifndef VIEW_H
#define VIEW_H
#include "entity.h"
#include "pool.h"
/**
 * @brief Multi-Component View.
 * Searches for the smallest pool of components and uses this pool
 * to check if an entity contains the other types as well.
 * Less performant than groups because it doesn't modify/sort the underlying pool,
 * but for that same reason it's probably also safer/easier to use for simple tasks.
 * A run-time assert will be called if trying to get an entity that doesn't belong to the view.
 *  * Only Registry is allowed to create Views.
 */
template <typename... Component>
class View {
private:
    using underlying_iterator_type = typename IPool::iterator;
    using unchecked_type = std::array<std::shared_ptr<const IPool>, (sizeof...(Component) - 1)>;
    friend class Registry;

    /**
     * @brief Returns the smallest pool of components for use with begin() and end() functions
     * @return
     */
    std::shared_ptr<const IPool> candidate() const {
        return std::min({static_cast<std::shared_ptr<const IPool>>(std::get<std::shared_ptr<Pool<Component>>>(pools))...}, [](const auto lhs, const auto rhs) {
            return lhs->size() < rhs->size();
        });
    }

    class iterator {
        friend class View<Component...>;

        iterator(unchecked_type other, underlying_iterator_type first, underlying_iterator_type last)
            : unchecked{other},
              begin{first},
              end{last} {
            if (begin != end && !valid()) {
                ++(*this);
            }
        }

        bool valid() const {
            return std::all_of(unchecked.cbegin(), unchecked.cend(), [this](std::shared_ptr<const IPool> view) {
                return view->has(*begin);
            });
        }

    public:
        using difference_type = typename underlying_iterator_type::difference_type;
        using pointer = typename underlying_iterator_type::pointer;
        using reference = typename underlying_iterator_type::reference;
        using iterator_category = std::forward_iterator_tag;

        iterator() = default;

        iterator &operator++() {
            return (++begin != end && !valid()) ? ++(*this) : *this;
        }

        iterator operator++(int) {
            iterator orig = *this;
            return ++(*this), orig;
        }

        bool operator==(const iterator &other) const {
            return other.begin == begin;
        }

        bool operator!=(const iterator &other) const {
            return !(*this == other);
        }

        pointer operator->() const {
            return begin.operator->();
        }

        reference operator*() const {
            return *operator->();
        }

    private:
        unchecked_type unchecked;
        underlying_iterator_type begin;
        underlying_iterator_type end;
    };
    unchecked_type unchecked(std::shared_ptr<const IPool> view) const {
        unchecked_type other{};
        typename unchecked_type::size_type pos{};
        ((std::get<std::shared_ptr<Pool<Component>>>(pools) == view ? nullptr : (other[pos++] = std::get<std::shared_ptr<Pool<Component>>>(pools))), ...);
        return other;
    }

public:
    template <typename Comp>
    size_t size() const {
        return std::get<std::shared_ptr<Pool<Comp>>>(pools)->size();
    }
    iterator begin() const {
        const auto view = candidate();
        return iterator{unchecked(view), view->begin(), view->end()};
    }
    iterator end() const {
        const auto view = candidate();
        return iterator{unchecked(view), view->end(), view->end()};
    }
    template <typename Comp>
    std::vector<Comp> &data() const {
        return std::get<Pool<Comp>>(pools)->data();
    }
    template <typename Comp>
    std::vector<int> &entities() const {
        return std::get<Pool<Comp>>(pools)->entities();
    }
    int find(const int &entt) const {
        return candidate()->find(entt);
    }
    bool contains(const int &entt) const {
        return find(entt) != -1;
    }
    template <typename... Comp>
    decltype(auto) get(const int &entt) {
        assert(contains(entt));
        if constexpr (sizeof...(Comp) == 1) {
            return (std::get<std::shared_ptr<Pool<Comp>>>(pools)->get(entt), ...);
        } else
            return std::tuple<decltype(get<Comp>(entt))...>{get<Comp>(entt)...};
    }

private:
    View(std::shared_ptr<Pool<Component>>... ref)
        : pools{ref...} {
    }
    std::tuple<std::shared_ptr<Pool<Component>>...> pools;
};
/**
 *@brief Single component view type.
 * No need to check for entity existence or anything else,
 * which makes this a useful and fast utility for accessing
 * all the components of a type.
 * Only Registry is allowed to create Views.
 */
template <typename Component>
class View<Component> {
private:
    friend class Registry;
    View(std::shared_ptr<Pool<Component>> ref)
        : pool{ref} {}
    std::shared_ptr<Pool<Component>> pool;

public:
    using iterator_type = typename IPool::iterator;
    size_t size() const {
        return std::shared_ptr<Pool<Component>>(pool)->size();
    }
    iterator_type begin() const {
        return pool->begin();
    }
    iterator_type end() const {
        return pool->end();
    }
    std::vector<Component> &data() const {
        return Pool<Component>(pool)->data();
    }
    std::vector<int> &entities() const {
        return Pool<Component>(pool)->entities();
    }
    int find(const int &entt) const {
        return pool->find(entt);
    }
    bool contains(const int &entt) const {
        return find(entt) != -1;
    }
    decltype(auto) get(const int &entt) {
        assert(contains(entt));
        return pool->get(entt);
    }
};
#endif // VIEW_H
