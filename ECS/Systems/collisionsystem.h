#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "components.h"
#include "registry.h"

class CollisionSystem {
public:
    CollisionSystem();

    Registry *reg = Registry::instance();

    vec3 getMin(const Collision::AABB &aabb);
    vec3 getMax(const Collision::AABB &aabb);

    // Need AABBSphere() and OBBSphere(), might use macro for swapping?
    bool AABBAABB(const Collision::AABB &aabb1, const Collision::AABB &aabb2);
    bool SphereAABB(const Collision::Sphere &sphere, const Collision::AABB &aabb1);
    // bool SphereOBB(const Collision::Sphere &sphere, const Collision::OBB &obb);
    bool SphereSphere(const Collision::Sphere &sphere1, const Collision::Sphere &sphere2);
    bool AABBPlane(const Collision::AABB &aabb, const Collision::Plane &plane);

    // Work in progress this guy
    bool CylinderCylinder(const Collision::Cylinder &cylinder1, const Collision::Cylinder &cylinder2);

    // Could this be useful later?
    // bool PointInAABB(const vec3& point, const Collision::AABB& aabb);
    // bool PointInSphere(const vec3& point, const Collision::Sphere& sphere);
    // bool PointInOBB(const vec3& point, const Collision::OBB& obb);

    vec3 ClosestPoint(const Collision::AABB &aabb, const vec3 &point);
    // todo
    vec3 ClosestPoint(const Collision::OBB &obb, const vec3 &point);
    vec3 ClosestPoint(const Collision::Sphere &sphere, const vec3 &point);
    bool SphereOBB(const Collision::Sphere &sphere, const Collision::OBB &obb);
};

#endif // COLLISIONSYSTEM_H
