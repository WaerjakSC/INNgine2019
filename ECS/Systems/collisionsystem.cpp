#include "collisionsystem.h"
#include "components.h"
#include "registry.h"
#include <cmath>

CollisionSystem::CollisionSystem() {
    Registry::instance()->registerComponent<Collision>();
}

bool CollisionSystem::IntersectOBB() {
}

/**bool CollisionSystem::IntersectAABBAABB(const std::pair<vec3, vec3> &one, const std::pair<vec3, vec3> two) {
    // Collision only if overlap on both axes
    return (one.first.x <= two.second.x && one.second.x >= two.first.x) &&
           (one.first.y <= two.second.y && one.second.y >= two.first.y) &&
           (one.first.z <= two.second.z && one.second.z >= two.first.z);
}
*/

/**
 * @brief Helper function CollisionSystem::getMin finds minimum point in an AABB
 * @param aabb
 * @return a vector minimum point found in the AABB
 */
vec3 CollisionSystem::getMin(const Collision::AABB &aabb) {
    vec3 p1 = aabb.origin + aabb.size;
    vec3 p2 = aabb.origin - aabb.size;

    return vec3(fminf(p1.x, p2.x),
                fminf(p1.y, p2.y),
                fminf(p1.z, p2.z));
}

/**
 * @brief Helper function CollisionSystem::getMax finds maximum point in an AABB
 * @param aabb
 * @return a vector maximum point found in the AABB
 */
vec3 CollisionSystem::getMax(const Collision::AABB &aabb) {
    vec3 p1 = aabb.origin + aabb.size;
    vec3 p2 = aabb.origin - aabb.size;

    return vec3(fmaxf(p1.x, p2.x),
                fmaxf(p1.y, p2.y),
                fmaxf(p1.z, p2.z));
}
/**
 * @brief bool CollisionSystem::AABBAABB returns true if an intersection between given AABB's occur
 * @param AABB1
 * @param AABB2
 * @return
 */
bool CollisionSystem::AABBAABB(const Collision::AABB &AABB1, const Collision::AABB &AABB2) {

    vec3 aMin = getMin(AABB1);
    vec3 aMax = getMax(AABB1);

    vec3 bMin = getMin(AABB2);
    vec3 bMax = getMax(AABB2);

    return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
            (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
            (aMin.z <= bMax.z && aMax.z >= bMin.z);
}

/**
 * @brief CollisionSystem::ClosestPoint finds the point in an AABB closest to a given point
 * @param Collider aabb
 * @param vec3 point
 * @return the vec3 closest point
 */
vec3 CollisionSystem::ClosestPoint(const Collision::AABB &aabb, const vec3 &point)
{
    vec3 result = point;
    vec3 min = getMin(aabb);
    vec3 max = getMax(aabb);

    // Clamping closest point to the minimum point in given AABB
    result.x = (result.x < min.x ? min.x : result.x);
    result.y = (result.y < min.y ? min.y : result.y);
    result.z = (result.z < min.z ? min.z : result.z);

    // Clamping closest point to the maximum point in given AABB
    result.x = (result.x > max.x ? max.x : result.x);
    result.y = (result.y > max.y ? max.y : result.y);
    result.z = (result.z > max.z ? max.z : result.z);

    return result;

}

/**
 * @brief CollisionSystem::SphereAABB collision between a Sphere and AABB
 * @param sphere
 * @param aabb
 * @return true if distance is less than radius (we have an intersection)
 */
bool CollisionSystem::SphereAABB(const Collision::Sphere &sphere, const Collision::AABB &aabb){
    vec3 closestPoint = ClosestPoint(aabb, sphere.radius);
    // not 100% sure about this one
    float dist = (sphere.position - closestPoint).length();
    float radiusSq = sphere.radius * sphere.radius;

    return dist < radiusSq;
}
