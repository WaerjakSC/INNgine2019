#ifndef CORE_H
#define CORE_H
#include "vector3d.h"
#include "vector4d.h"
#include <memory>

namespace cjk {
// Simplify for ease of typing
template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T>
using Ref = std::shared_ptr<T>;

using vec3 = gsl::Vector3D;

/**
 * @brief The CType enum holds component type info to quickly check if an entity contains a certain component type.
 */
enum class CType {
    None = 0,
    Transform = 1 << 0,
    Material = 1 << 1,
    Mesh = 1 << 2,
    Light = 1 << 3,
    Input = 1 << 4,
    Physics = 1 << 5,
    Sound = 1 << 6,
    Collision = 1 << 7
};
} // namespace cjk
// ******** Helper functions for CType ******** //
template <typename E>
struct enableBitmaskOperators {
    static constexpr bool enable = false;
};
template <>
struct enableBitmaskOperators<cjk::CType> {
    static constexpr bool enable = true;
};
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E>::type
operator|(E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E>::type
operator~(E type) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(static_cast<underlying>(type));
}
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E &>::type
operator|=(E &lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    lhs = static_cast<E>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    return lhs;
}
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E &>::type
operator&=(E &lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    lhs = static_cast<E>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    return lhs;
}
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E>::type
operator&(E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}
#endif // CORE_H
