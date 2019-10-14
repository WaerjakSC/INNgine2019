#include "collisionsystem.h"
#include "components.h"
#include "registry.h"

CollisionSystem::CollisionSystem()
{

}


bool CollisionSystem::IntersectOBB()
{

}


bool CollisionSystem::IntersectAABBAABB(const std::pair<vec3, vec3> &one, const std::pair<vec3, vec3> two)
{
    // Collision only if overlap on both axes
    return (one.first.x <= two.second.x && one.second.x >= two.first.x) &&
            (one.first.y <= two.second.y && one.second.y >= two.first.y) &&
            (one.first.z <= two.second.z && one.second.z >= two.first.z);
}

/**
 * @brief Helper function CollisionSystem::getMin finds minimum point in an AABB
 * @param aabb
 * @return a vector minimum point found in the AABB
 */

vec3 CollisionSystem::getMin(const Collision::AABB &aabb)
{
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

vec3 CollisionSystem::getMax(const Collision::AABB &aabb)
{
    vec3 p1 = aabb.origin + aabb.size;
    vec3 p2 = aabb.origin - aabb.size;

    return vec3(fmaxf(p1.x, p2.x),
                fmaxf(p1.y, p2.y),
                fmaxf(p1.z, p2.z));
}

bool CollisionSystem::AABBAABB(const Collision::AABB& AABB1, const Collision::AABB& AABB2)
{

    vec3 aMin = getMin(AABB1);
    vec3 aMax = getMax(AABB1);

    vec3 bMin = getMin(AABB2);
    vec3 bMax = getMax(AABB2);

    return  (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
            (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
            (aMin.z <= bMax.z && aMax.z >= bMin.z);

}


