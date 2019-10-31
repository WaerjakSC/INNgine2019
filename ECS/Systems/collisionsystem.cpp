#include "collisionsystem.h"
#include "components.h"
#include "registry.h"
#include <cmath>

CollisionSystem::CollisionSystem() {
    Registry::instance()->registerComponent<Collision>();
}

void CollisionSystem::update(float deltaTime) {
}

/**
 * @brief Helper function CollisionSystem::getMin finds minimum point in an AABB
 * @param aabb
 * @return a vector minimum point found in the AABB
 */
vec3 CollisionSystem::getMin(const AABB &aabb) {
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
vec3 CollisionSystem::getMax(const AABB &aabb) {
    vec3 p1 = aabb.origin + aabb.size;
    vec3 p2 = aabb.origin - aabb.size;

    return vec3(fmaxf(p1.x, p2.x),
                fmaxf(p1.y, p2.y),
                fmaxf(p1.z, p2.z));
}

/**
 * @brief CollisionSystem::SphereOBB returns true if an intersection between a Sphere and an OBB occurs
 * @param sphere
 * @param obb
 * @return true if distance less than radius squared ( We have an intersection )
 */
bool CollisionSystem::SphereOBB(const Sphere &sphere, const OBB &obb) {

    // Finner først nærmeste punktet i OBB til sphere center
    vec3 p = ClosestPoint(obb, sphere.position);
    // Finner avstanden mellom sphere center og punktet i OBB
    float dist = (sphere.position - p).length();
    float radiusSq = sphere.radius * sphere.radius;
    // Hvis avstanden er mindre en radius^2, har vi en intersection mellom Sphere og OBB
    return dist < radiusSq;
}

void CollisionSystem::DrawColliders()
{
    initializeOpenGLFunctions();
    auto view = reg->view<Transform, Collision>();
    for(auto entityID : view){
        if(reg->contains<Collision>(entityID)){
            auto &collider = view.get<Collision>(entityID);
            glBindVertexArray(0);
            glBindVertexArray(collider.mVAO);
            glDrawElements(GL_LINE_LOOP, 16, GL_UNSIGNED_SHORT, nullptr); // Might use GL_STRIP instead, not sure yet
        }
    }
}

/**
 * @brief CollisionSystem::SphereSphere
 * @param sphere1
 * @param sphere2
 * @return true if distance is less than radius^2 (intersection)
 */
bool CollisionSystem::SphereSphere(const Sphere &sphere1, const Sphere &sphere2) {
    // sum of radius
    float rs = sphere1.radius + sphere2.radius;
    // dist squared
    float dist = (sphere1.position + sphere2.position).length();
    // compare
    return dist < (rs * rs);
}

/**
 * @brief CollisionSystem::AABBPlane returns true if an intersection between an AABB and a plane occurs
 * @param aabb
 * @param plane
 * @return
 */
bool CollisionSystem::AABBPlane(const AABB &aabb, const Plane &plane) { // WIP - not sure if this works
    float mHalfExtent = aabb.size.x * fabsf(plane.normal.x) +
            aabb.size.y * fabsf(plane.normal.y) +
            aabb.size.z * fabsf(plane.normal.z);

    // Distance from center of AABB to plane
    float dotProduct = vec3::dot(plane.normal, aabb.origin);
    float dist = dotProduct - plane.distance;

    return fabsf(dist) <= mHalfExtent;
}

/**
 * @brief CollisionSystem::SphereAABB collision between a Sphere and AABB
 * @param sphere
 * @param aabb
 * @return true if distance is less than radius (we have an intersection)
 */
bool CollisionSystem::SphereAABB(const Sphere &sphere, const AABB &aabb) {
    vec3 closestPoint = ClosestPoint(aabb, sphere.radius);
    // not 100% sure about this one
    float dist = (sphere.position - closestPoint).length();
    float radiusSq = sphere.radius * sphere.radius;

    return dist < radiusSq;
}
/**
 * @brief bool CollisionSystem::AABBAABB returns true if an intersection between given AABB's occur
 * @param AABB1
 * @param AABB2
 * @return true if all axis overlap (we have an intersection)
 */
bool CollisionSystem::AABBAABB(const AABB &AABB1, const AABB &AABB2) {

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
vec3 CollisionSystem::ClosestPoint(const AABB &aabb, const vec3 &point) {
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
 * @brief CollisionSystem::ClosestPoint finds the point in an OBB closest to a given point
 * @param obb
 * @param point
 * @return
 */
vec3 CollisionSystem::ClosestPoint(const OBB &obb, const vec3 &point) {
    vec3 result = obb.position;
    //     move the point relative to the OBB
    vec3 dir = point - obb.position;

    // Loops three times, once for each axis: #0 for the X-axis, #1 for the Y-axis, #2 for the Z-axis
    // projects the point onto each of the axes of the box,
    // and compares the distance to the extent of the box
    for (int i = 0; i < 3; ++i) {
        const float *orientation = &obb.orientation.matrix[i * 3];
        // vector that holds the different axis
        vec3 axis(orientation[0],
                orientation[1],
                orientation[2]);
        // projects the point onto that axis and stores the distance
        float dist = vec3::dot(dir, axis);

        // clamp
        if (dist > obb.size[i]) {
            dist = obb.size[i];
        }
        if (dist < -obb.size[i]) {
            dist = -obb.size[i];
        }

        // adjust the point by the axis and the distance
        result = result + (axis * dist);
    }

    return result;
}

/**
 * @brief CollisionSystem::ClosestPoint finds the point in a Sphere closest to a given point
 * @param sphere
 * @param point
 * @return resized vector, offset by sphere.position
 */
vec3 CollisionSystem::ClosestPoint(const Sphere &sphere, const vec3 &point) {
    vec3 sphereCenterToPoint = point - sphere.position;
    sphereCenterToPoint.normalize();
    sphereCenterToPoint = sphereCenterToPoint * sphere.radius;

    return sphereCenterToPoint + sphere.position;
}
