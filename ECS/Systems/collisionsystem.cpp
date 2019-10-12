#include "collisionsystem.h"
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


