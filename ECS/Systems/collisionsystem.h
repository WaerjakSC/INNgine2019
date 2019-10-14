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

    // Need AABBSphere() and OBBSphere(), might use macro for swapping?
    bool AABBAABB(const Collision::AABB& aabb1, const Collision::AABB& aabb2);
    bool SphereAABB(const Collision::Sphere& sphere, const Collision::AABB& aabb1);
    // todo
    bool SphereOBB(const Collision::Sphere& sphere, const Collision::OBB& obb);
    bool SphereSphere(const Collision::Sphere& sphere1, const Collision::Sphere& sphere2);


    // Could this be useful later?
    // bool PointInAABB(const vec3& point, const Collision::AABB& aabb);
    // bool PointInSphere(const vec3& point, const Collision::Sphere& sphere);
    // bool PointInOBB(const vec3& point, const Collision::OBB& obb);

    vec3 ClosestPoint(const Collision::AABB& aabb, const vec3& point);
    // todo
    vec3 ClosestPoint(const Collision::OBB& obb, const vec3& point);
    vec3 ClosestPoint(const Collision::Sphere& sphere, const vec3& point);


    Collision::AABB *aa;

};

#endif // COLLISIONSYSTEM_H
