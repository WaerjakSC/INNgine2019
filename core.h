#ifndef CORE_H
#define CORE_H
#include <memory>

namespace cjk {
// Simplify for ease of typing
/**
 * Engine-specific unique pointer.
 */
template <typename T>
using Scope = std::unique_ptr<T>;

/**
 * Engine-specific shared_ptr.
 */
template <typename T>
using Ref = std::shared_ptr<T>;

} // namespace cjk
#endif // CORE_H
