#include "collisionsystem.h"
#include "components.h"
#include "raycast.h"
#include "registry.h"
#include <cmath>
CollisionSystem::CollisionSystem() : reg(Registry::instance()) {
}

void CollisionSystem::update(DeltaTime dt) {
    delta += dt;
    if (delta >= fixedDelta) {
        delta = 0.f;
        runSimulations();
    }
}
void CollisionSystem::runSimulations() {
    runAABBSimulations();
    runSphereSimulations();
}
void CollisionSystem::runAABBSimulations() {
    auto view = reg->view<AABB>();
    auto sphereView = reg->view<Sphere>();
    for (auto entity : view) {
        auto &aabb = view.get(entity);
        // Check for AABB-AABB intersections
        for (auto otherEntity : view) {
            if (entity != otherEntity) {
                auto &otherAABB = view.get(otherEntity);
                if (!bothStatic(aabb, otherAABB))
                    if (AABBAABB(aabb, otherAABB)) {
                        //                aabbAIcomponent.hp -= sphereAIcomponent.damage;
                        QString entity1 = reg->getEntity(entity)->name();
                        QString entity2 = reg->getEntity(otherEntity)->name();
                        //                        qDebug() << "Collision between " + entity1 + " and " + entity2 + " " + QString::number(collisions);
                        collisions++;
                        // notify FSM if needed
                    }
            }
        }
        for (auto otherEntity : sphereView) {
            if (entity != otherEntity) {
                auto &sphere = sphereView.get(otherEntity);
                if (!bothStatic(aabb, sphere))
                    if (SphereAABB(sphere, aabb)) {
                        //                aabbAIcomponent.hp -= sphereAIcomponent.damage;
                        QString entity1 = reg->getEntity(entity)->name();
                        QString entity2 = reg->getEntity(otherEntity)->name();
                        if (entity1 == "Enemy" && entity2 == "Player") {
                            qDebug() << "oops you got hit by the red ogre!";
                        }
                        //                        qDebug() << "Collision between " + entity1 + " and " + entity2 + " " + QString::number(collisions);
                        collisions++;
                        // notify FSM if needed
                    }
            }
        }
    }
}
void CollisionSystem::runSphereSimulations() {
    auto view = reg->view<Sphere>();
    for (auto entity : view) {
        auto &sphere = view.get(entity);
        for (auto otherEntity : view) {
            if (entity != otherEntity) {
                auto &otherSphere = view.get(otherEntity);
                if (!bothStatic(sphere, otherSphere))
                    if (SphereSphere(sphere, otherSphere)) {
                        //                sphereAIcomponent.hp -= otherSphereAIcomponent.damage;
                        QString entity1 = reg->getEntity(entity)->name();
                        QString entity2 = reg->getEntity(otherEntity)->name();

                        //                        qDebug() << "Collision between " + entity1 + " and " + entity2 + " " + QString::number(collisions);
                        collisions++;
                        // notify FSM if needed
                    }
            }
        }
    }
}
bool CollisionSystem::bothStatic(const Collision &lhs, const Collision &rhs) {
    return (lhs.isStatic == rhs.isStatic) == true;
}

vec3 CollisionSystem::getMin(const AABB &aabb) {
    vec3 origin = aabb.transform.modelMatrix.getPosition();
    vec3 p1 = origin + aabb.size;
    vec3 p2 = origin - aabb.size;

    return vec3(fminf(p1.x, p2.x),
                fminf(p1.y, p2.y),
                fminf(p1.z, p2.z));
}

vec3 CollisionSystem::getMax(const AABB &aabb) {
    vec3 origin = aabb.transform.modelMatrix.getPosition();
    vec3 p1 = origin + aabb.size;
    vec3 p2 = origin - aabb.size;

    return vec3(fmaxf(p1.x, p2.x),
                fmaxf(p1.y, p2.y),
                fmaxf(p1.z, p2.z));
}

bool CollisionSystem::SphereOBB(const Sphere &sphere, const OBB &obb) {

    // Finner først nærmeste punktet i OBB til sphere center
    vec3 p = ClosestPoint(obb, sphere.position);
    // Finner avstanden mellom sphere center og punktet i OBB
    float dist = (sphere.position - p).length();
    float radiusSq = sphere.radius * sphere.radius;
    // Hvis avstanden er mindre en radius^2, har vi en intersection mellom Sphere og OBB
    return dist < radiusSq;
}

bool CollisionSystem::SphereSphere(const Sphere &sphere1, const Sphere &sphere2) {
    vec3 sphere1Pos = sphere1.transform.modelMatrix.getPosition();
    vec3 sphere2Pos = sphere2.transform.modelMatrix.getPosition();
    // sum of radius
    float rs = sphere1.radius + sphere2.radius;
    // dist squared
    float dist = (sphere1Pos + sphere2Pos).length();
    // compare
    return dist < (rs * rs);
}

bool CollisionSystem::AABBPlane(const AABB &aabb, const Plane &plane) { // WIP - not sure if this works
    float mHalfExtent = aabb.size.x * fabsf(plane.normal.x) +
                        aabb.size.y * fabsf(plane.normal.y) +
                        aabb.size.z * fabsf(plane.normal.z);
    vec3 aabbPos = aabb.transform.modelMatrix.getPosition();
    // Distance from center of AABB to plane
    float dotProduct = vec3::dot(plane.normal, aabbPos);
    float dist = dotProduct - plane.distance;

    return fabsf(dist) <= mHalfExtent;
}

bool CollisionSystem::SphereAABB(const Sphere &sphere, const AABB &aabb) {
    // Get the actual position of the sphere - sphere.position only holds the offset from the entity it belongs to
    vec3 spherePos = sphere.transform.modelMatrix.getPosition();
    // Not sure what ClosestPoint aims to achieve, previously I think it was sending a vec3(sphere.radius, 0,0) due to how vector3d works
    //    vec3 closestPoint = ClosestPoint(aabb, sphere.radius);
    // Now sends the center position of the sphere
    vec3 closestPoint = ClosestPoint(aabb, spherePos);
    // not 100% sure about this one
    float dist = (spherePos - closestPoint).length();
    float radiusSq = sphere.radius * sphere.radius;

    return dist < radiusSq;
}

bool CollisionSystem::AABBAABB(const AABB &AABB1, const AABB &AABB2) {
    vec3 aMin = getMin(AABB1);
    vec3 aMax = getMax(AABB1);

    vec3 bMin = getMin(AABB2);
    vec3 bMax = getMax(AABB2);

    return aMin <= bMax && aMax >= bMin;
}

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

vec3 CollisionSystem::ClosestPoint(const Sphere &sphere, const vec3 &point) {
    vec3 spherePos = sphere.transform.modelMatrix.getPosition();
    vec3 sphereCenterToPoint = point - spherePos;
    sphereCenterToPoint.normalize();
    sphereCenterToPoint = sphereCenterToPoint * sphere.radius;

    return sphereCenterToPoint + spherePos;
}

bool CollisionSystem::RayToSphere(const Ray &ray, const Sphere &sphere, double &intersectionDistance) {
    vec3 center = sphere.transform.modelMatrix.getPosition();
    vec3 originToCenter = ray.origin - center;

    float a = vec3::dot(ray.direction, ray.direction);
    float b = 2.0 * vec3::dot(originToCenter, ray.direction);
    float c = vec3::dot(originToCenter, originToCenter) - (sphere.radius * sphere.radius);
    float discriminant = (b * b) - (4 * a * c);
    if (discriminant < 0)
        return false;
    if (discriminant > 0.0f) {
        // get the 2 intersection distances along ray
        double t_a = (-b + sqrt(discriminant) / (2.0 * a));
        double t_b = (-b - sqrt(discriminant) / (2.0 * a));
        intersectionDistance = t_b;
        // if behind viewer, throw one or both away
        if (t_a < 0.0f) {
            if (t_b < 0.0f)
                return false;
        } else if (t_b < 0.0f)
            intersectionDistance = t_a;
        return true;
    }
    // check for ray hitting once (skimming the surface)
    if (0.0f == discriminant) {
        // if behind viewer, throw away
        double t = (-b + sqrt(discriminant) / (2.0 * a));
        if (t < 0.0f)
            return false;
        intersectionDistance = t;
        return true;
    }
    return false;
}

bool CollisionSystem::RayToAABB(const Ray &r, const AABB &aabb, double &intersectionDistance) {
    vec3 AABBmin = getMin(aabb);
    vec3 AABBmax = getMax(aabb);

    float t1 = (AABBmin.x - r.origin.x) * r.invDir.x;
    float t2 = (AABBmax.x - r.origin.x) * r.invDir.x;

    float tmin = std::min(t1, t2);
    float tmax = std::max(t1, t2);

    for (int i = 1; i < 3; ++i) {
        t1 = (AABBmin[i] - r.origin[i]) * r.invDir[i];
        t2 = (AABBmax[i] - r.origin[i]) * r.invDir[i];

        tmin = std::max(tmin, std::min(t1, t2));
        tmax = std::min(tmax, std::max(t1, t2));
    }

    if (tmax >= std::max(tmin, 0.f)) {
        intersectionDistance = tmax; // This might need to be tmin instead
        return true;
    }
    return false;
}
