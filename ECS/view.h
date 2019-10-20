#ifndef VIEW_H
#define VIEW_H
#include "entity.h"
#include "pool.h"
template <typename... Component>
class View {
public:
    View(std::shared_ptr<Pool<Component>>... ref)
        : pools{ref...} {
    }

    template <typename... Comp>
    decltype(auto) get(const Entity entt) const {
        if constexpr (sizeof...(Comp) == 1) {
            return (std::get<std::shared_ptr<Pool<Comp>>>(pools)->get(entt.id()), ...);
        } else
            return std::tuple<decltype(get<Comp>(entt))...>{get<Comp>(entt)...};
    }

private:
    const std::tuple<std::shared_ptr<Pool<Component>>...> pools;
};

#endif // VIEW_H
