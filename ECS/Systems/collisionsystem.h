#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "components.h"
#include "deltaTime.h"
#include "isystem.h"
#include <QOpenGLFunctions_4_1_Core>
struct Ray {
    Ray() {}
    Ray(const vec3 &orig, const vec3 &dir) : origin{orig}, direction{dir} {
        invDir = direction.divide(1.f);
        sign[0] = (invDir.x < 0);
        sign[1] = (invDir.y < 0);
        sign[2] = (invDir.z < 0);
    }
    vec3 origin;
    vec3 direction;
    vec3 invDir;
    int sign[3];
};
struct Raycast {
    Raycast(float range) : rayRange(range), closestTarget(range) {
    }

    Ray ray;
    float rayRange;
    double intersectionDistance;
    double closestTarget;
    int hitEntity{-1};
    vec3 hitPoint;
};
class Registry;
class CollisionSystem : public QObject, public ISystem, public QOpenGLFunctions_4_1_Core {
    Q_OBJECT
public:
    CollisionSystem();

    void update(DeltaTime dt = 0.016) override;
    /**
     * @brief mousePick creates a raycast object with the given range parameter and tries to pick a point in the viewpoint Rect from mousePos (x, y).
     * @param mousePos
     * @param rect
     * @param ignoredEntity Can be supplied to force the function to ignore a certain entity, such as the object being dragged in InputSystem's dragEntity()
     * @param range
     * @return
     */
    Raycast mousePick(const QPoint &mousePos, const QRect &rect, int ignoredEntity = -1, float range = 250.f);
public slots:
    void setOriginX(double xIn);
    void setOriginY(double xIn);
    void setOriginZ(double xIn);
    void setAABBSizeX(double xIn);
    void setAABBSizeY(double yIn);
    void setAABBSizeZ(double zIn);
    void setSpherePositionX(double xIn);
    void setSpherePositionY(double yIn);
    void setSpherePositionZ(double zIn);
    void setSphereRadius(double radius);
    void setPlaneNormalX(double xIn);
    void setPlaneNormalY(double yIn);
    void setPlaneNormalZ(double zIn);
    void setPlaneDistance(double radius);
    void setObjectType(int index);
signals:
    void updateAABB(GLuint eID);
    void updateSphere(GLuint eID);

private:
    float fixedDelta{0.02f};
    float delta{0};
    int collisions{0};
    Registry *registry;
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
    /**
     * @brief rayAABB checks every entity owning an AABB collider to find the closest one hit by the ray.
     * @param ray
     * @param ignoredEntity
     */
    void rayAABB(Raycast &ray, int ignoredEntity = -1);
    /**
     * @brief raySphere checks every entity owning a Sphere collider to find the closest one hit by the ray.
     * @param ray
     * @param ignoredEntity
     */
    void raySphere(Raycast &ray, int ignoredEntity = -1);

    void rayPlane(Raycast &ray, int ignoredEntity = -1);

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
    bool AABBPlane(const AABB &aabb, Plane &plane);
    /**
    * @brief SphereAABB collision between a Sphere and AABB
    * @param sphere
    * @param aabb
    * @return true if distance is less than radius (we have an intersection)
    */
    bool SphereAABB(const Sphere &sphere, const AABB &aabb1);
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
    //    bool SphereOBB(const Sphere &sphere, const OBB &obb);
    /**
     * @brief calcRayToSphere finds the intersection point between a ray and a sphere collider
     * Not to be confused with raySphere which runs this function for each entity
     * @param ray
     * @param sphere
     * @param intersectionDistance retrieves the distance between the ray origin and the sphere
     * @return true if hit, false if not
     */
    bool calcRayToSphere(Raycast &r, const Sphere &sphere);
    /**
     * @brief calcRayToAABB finds the intersection point between ray and AABB collider
     * Not to be confused with rayAABB which runs this function for each entity
     * @param r
     * @param aabb
     * @param intersectionDistance retrieves the distance between ray origin and AABB
     * @return true if hit, false if not
     */
    bool calcRayToAABB(Raycast &r, const AABB &aabb);
    /**
     * @brief getRayFromMouse calculates a ray based on the mouse's position in the viewport
     * @param mousePos
     * @param rect
     * @return
     */
    Ray getRayFromMouse(const QPoint &mousePos, const QRect &rect);
    /**
     * @brief getPointOnRay calculates a point on a ray given a specific distance from the ray's origin
     * @param r
     * @param distance
     * @return
     */
    vec3 getPointOnRay(const Raycast &r, double distance);
    // Work in progress this guy
    //    bool CylinderCylinder(const Cylinder &cylinder1, const Cylinder &cylinder2);

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
    /**
    * @brief ClosestPoint finds the point in a Sphere closest to a given point
    * @param sphere collider type
    * @param point
    * @return resized vector, offset by sphere.position
    */
    vec3 ClosestPoint(const Sphere &sphere, const vec3 &point);
    // todo
    //    vec3 ClosestPoint(const OBB &obb, const vec3 &point);
    /**
     * @brief bothStatic simple bool check to see if both Collision objects have isStatic set to true
     * @param lhs
     * @param rhs
     * @return if both are static objects, return true
     */
    bool bothStatic(const Collision &lhs, const Collision &rhs);
    bool calcRayToPlane(Raycast &r, Plane &plane);
    float distanceToPoint(const Plane &plane, const vec3 &point) const;
    float calcDistance(Plane &plane);
};

#endif // COLLISIONSYSTEM_H
