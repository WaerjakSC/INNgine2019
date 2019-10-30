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

} // namespace cjk

#endif // CORE_H
