#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "registry.h"
#include "components.h"

class CollisionSystem
{
public:
    CollisionSystem();

  //  bool IntersectAABB(int eID, int otherEID);
    bool IntersectOBB();
    Registry *reg = Registry::instance();
    static bool IntersectAABBAABB(const std::pair<vec3, vec3>& a, const std::pair<vec3, vec3>b);

    vec3 getMin(const Collision::AABB& aabb);
    vec3 getMax(const Collision::AABB& aabb);


    bool AABBAABB(const Collision::AABB& aabb1, const Collision::AABB& aabb2);
    bool SphereAABB(const Collision::Sphere& sphere, const Collision::AABB& aabb1);

    bool PointInAABB(const vec3& point, const Collision::AABB& aabb);
    vec3 ClosestPoint(const Collision::AABB& aabb, const vec3& point);

    Collision::AABB *aa;

};

#endif // COLLISIONSYSTEM_H
