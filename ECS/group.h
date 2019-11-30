#ifndef GROUP_H
#define GROUP_H
#include "pool.h"
template <typename Owned, typename... Other>
class Group {
public:
    Group(Owned *owned, Other *... other) : pools{owned, other...} {
    }

private:
    friend class Registry;
    const std::tuple<Pool<Owned> *, Pool<Other> *...> pools;
};

#endif // GROUP_H
