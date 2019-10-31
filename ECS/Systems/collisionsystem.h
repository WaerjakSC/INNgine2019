#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "components.h"
#include "registry.h"
#include "isystem.h"
#include <QOpenGLFunctions_4_1_Core>
class CollisionSystem : public ISystem, public QOpenGLFunctions_4_1_Core {
public:
    CollisionSystem();

    Registry *reg = Registry::instance();
    void update(float deltaTime = 0.016) override;

    vec3 getMin(const AABB &aabb);
    vec3 getMax(const AABB &aabb);

    // Need AABBSphere() and OBBSphere(), might use macro for swapping?
    bool AABBAABB(const AABB &aabb1, const AABB &aabb2);
    bool SphereAABB(const Sphere &sphere, const AABB &aabb1);
    // bool SphereOBB(const Collision::Sphere &sphere, const Collision::OBB &obb);
    bool SphereSphere(const Sphere &sphere1, const Sphere &sphere2);
    bool AABBPlane(const AABB &aabb, const Plane &plane);

    // Work in progress this guy
    bool CylinderCylinder(const Cylinder &cylinder1, const Cylinder &cylinder2);

    // Could this be useful later?
    // bool PointInAABB(const vec3& point, const Collision::AABB& aabb);
    // bool PointInSphere(const vec3& point, const Collision::Sphere& sphere);
    // bool PointInOBB(const vec3& point, const Collision::OBB& obb);

    vec3 ClosestPoint(const AABB &aabb, const vec3 &point);
    // todo
    vec3 ClosestPoint(const OBB &obb, const vec3 &point);
    vec3 ClosestPoint(const Sphere &sphere, const vec3 &point);
    bool SphereOBB(const Sphere &sphere, const OBB &obb);

    void DrawColliders();
};

#endif // COLLISIONSYSTEM_H
