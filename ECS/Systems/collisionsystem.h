#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "components.h"
#include "deltaTime.h"
#include "isystem.h"
#include <QOpenGLFunctions_4_1_Core>
struct Ray;
class Registry;
class CollisionSystem : public ISystem, public QOpenGLFunctions_4_1_Core {
public:
    CollisionSystem();

    void update(DeltaTime dt = 0.016) override;

private:
    float fixedDelta{0.02f};
    float delta{0};
    int collisions{0};
    Registry *reg;
    /**
     * @brief runSimulations simple function to run all the "Simulation" type functions in CollisionSystem
     */
    void runSimulations();
    /**
     * @brief runAABBSimulations runs the collision simulations for AABB types
     */
    void runAABBSimulations();
    /**
     * @brief runSphereSimulations runs the collision simulations for sphere based collider types
     */
    void runSphereSimulations();
    /**
    * @brief Helper function getMin finds minimum point in an AABB
    * @param aabb
    * @return a vector minimum point found in the AABB
    */
    vec3 getMin(const AABB &aabb);
    /**
    * @brief Helper function getMax finds maximum point in an AABB
    * @param aabb
    * @return a vector maximum point found in the AABB
    */
    vec3 getMax(const AABB &aabb);

    // Need AABBSphere() and OBBSphere(), might use macro for swapping?
    /**
    * @brief AABBAABB returns true if an intersection between given AABB's occur
    * @param AABB1
    * @param AABB2
    * @return true if all axis overlap (we have an intersection)
    */
    bool AABBAABB(const AABB &aabb1, const AABB &aabb2);
    /**
    * @brief AABBPlane returns true if an intersection between an AABB and a plane occurs
    * @param aabb
    * @param plane
    * @return
     */
    bool AABBPlane(const AABB &aabb, const Plane &plane);
    /**
    * @brief SphereAABB collision between a Sphere and AABB
    * @param sphere
    * @param aabb
    * @return true if distance is less than radius (we have an intersection)
    */
    bool SphereAABB(const Sphere &sphere, const AABB &aabb1);
    // bool SphereOBB(const Collision::Sphere &sphere, const Collision::OBB &obb);
    /**
    * @brief SphereSphere
    * @param sphere1
    * @param sphere2
    * @return true if distance is less than radius^2 (intersection)
    */
    bool SphereSphere(const Sphere &sphere1, const Sphere &sphere2);
    /**
    * @brief SphereOBB returns true if an intersection between a Sphere and an OBB occurs
    * @param sphere
    * @param obb
    * @return true if distance less than radius squared ( We have an intersection )
    */
    bool SphereOBB(const Sphere &sphere, const OBB &obb);
    /**
     * @brief RayToSphere finds the intersection point between a ray and a sphere collider
     * @param ray
     * @param sphere
     * @param intersectionDistance retrieves the distance between the ray origin and the sphere
     * @return true if hit, false if not
     */
    bool RayToSphere(const Ray &ray, const Sphere &sphere, double &intersectionDistance);
    /**
     * @brief RayToAABB finds the intersection point between ray and AABB collider
     * @param r
     * @param aabb
     * @param intersectionDistance retrieves the distance between ray origin and AABB
     * @return true if hit, false if not
     */
    bool RayToAABB(const Ray &r, const AABB &aabb, double &intersectionDistance);
    // Work in progress this guy
    bool CylinderCylinder(const Cylinder &cylinder1, const Cylinder &cylinder2);

    // Could this be useful later?
    // bool PointInAABB(const vec3& point, const Collision::AABB& aabb);
    // bool PointInSphere(const vec3& point, const Collision::Sphere& sphere);
    // bool PointInOBB(const vec3& point, const Collision::OBB& obb);
    /**
    * @brief ClosestPoint finds the point in an AABB closest to a given point
    * @param Collider aabb
    * @param vec3 point
    * @return the vec3 closest point
    */
    vec3 ClosestPoint(const AABB &aabb, const vec3 &point);
    // todo
    vec3 ClosestPoint(const OBB &obb, const vec3 &point);
    /**
    * @brief ClosestPoint finds the point in a Sphere closest to a given point
    * @param sphere collider type
    * @param point
    * @return resized vector, offset by sphere.position
    */
    vec3 ClosestPoint(const Sphere &sphere, const vec3 &point);

    /**
     * @brief bothStatic simple bool check to see if both Collision objects have isStatic set to true
     * @param lhs
     * @param rhs
     * @return if both are static objects, return true
     */
    bool bothStatic(const Collision &lhs, const Collision &rhs);
    friend class Raycast;
};

#endif // COLLISIONSYSTEM_H
