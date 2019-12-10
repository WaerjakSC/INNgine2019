#include "collisionsystem.h"
#include "aisystem.h"
#include "cameracontroller.h"
#include "components.h"
#include "inputsystem.h"
#include "registry.h"
#include <cmath>

CollisionSystem::CollisionSystem() : registry{Registry::instance()}
{
}
void CollisionSystem::update(DeltaTime)
{
}
void CollisionSystem::updatePlayOnly(DeltaTime deltaTime)
{
    delta += deltaTime;
    if (delta >= fixedDelta) {
        delta = 0.f;
        runAABBSimulations();
        runSphereSimulations();
    }
}

void CollisionSystem::runAABBSimulations()
{
    // possible groups - AABB + AIComponent, do two passes, one for Buildable + AABB, another for AABB + Bullet or whatever
    auto view{registry->view<AIComponent, AABB>()};
    for (auto entity : view) {
        auto &aabb{view.get<AABB>(entity)};
        auto towerRangeView{registry->view<TowerComponent, Sphere>()};
        for (auto otherEntity : towerRangeView) {
            if (entity != otherEntity) {
                auto &sphere{towerRangeView.get<Sphere>(otherEntity)};
                if (!bothStatic(aabb, sphere))
                    if (SphereAABB(sphere, aabb)) {
                        //                aabbAIcomponent.hp -= sphereAIcomponent.damage;                     
                        // NOTIFY FSM
                        // send event notify ON ENTER (hei se på meg noe er i radius jippi)
                        // sjekker for overlaps mot tårnets radius(sphere)
                        if (aabb.overlapEvent && sphere.overlapEvent) {
                            if (!aabb.overlappedEntities.contains(otherEntity)) {
                                aabb.overlappedEntities.insert(otherEntity);
                            }
                        }

                        collisions++;
                        // notify FSM if needed
                    }
            }
        }
    }
}
void CollisionSystem::runSphereSimulations()
{
    auto view{registry->view<EInfo, Sphere>()};
    for (auto entity : view) {
        auto &sphere{view.get<Sphere>(entity)};
        for (auto otherEntity : view) {
            if (entity != otherEntity) {
                auto &otherSphere{view.get<Sphere>(otherEntity)};
                if (!bothStatic(sphere, otherSphere))
                    if (SphereSphere(sphere, otherSphere)) {
                        QString entity1{view.get<EInfo>(entity).name};
                        QString entity2{view.get<EInfo>(otherEntity).name};

                        collisions++;
                        // notify FSM if needed
                    }
            }
        }
    }
}
void CollisionSystem::rayAABB(Raycast &ray, int ignoredEntity)
{
    auto view{registry->view<AABB>()};
    for (auto entity : view) {
        if ((int)entity == ignoredEntity)
            continue;
        auto &aabb{view.get(entity)};
        if (calcRayToAABB(ray, aabb)) {
            if (ray.intersectionDistance < ray.closestTarget) {
                ray.closestTarget = ray.intersectionDistance;
                ray.hitEntity = entity;
            }
        }
    }
}
void CollisionSystem::raySphere(Raycast &ray, int ignoredEntity)
{
    auto view{registry->view<Sphere>()};
    for (auto entity : view) {
        if ((int)entity == ignoredEntity)
            continue;
        auto &sphere{view.get(entity)};
        if (calcRayToSphere(ray, sphere)) {
            if (ray.intersectionDistance < ray.closestTarget) {
                ray.closestTarget = ray.intersectionDistance;
                ray.hitEntity = entity;
            }
        }
    }
}

Raycast CollisionSystem::mousePick(const QPoint &mousePos, const QRect &rect, int ignoredEntity, float range)
{
    Raycast raycast{range};
    raycast.ray = getRayFromMouse(mousePos, rect);

    rayAABB(raycast, ignoredEntity);
    //    raySphere(raycast, ignoredEntity); // we probably don't care about sphere mouse picking.
    raycast.hitPoint = getPointOnRay(raycast, raycast.closestTarget);

    if (raycast.hitEntity == -1)
        raycast.hitPoint = getPointOnRay(raycast, raycast.rayRange);
    return raycast;
}

gsl::Vector3D CollisionSystem::getPointOnRay(const Raycast &r, double distance)
{
    return r.ray.origin + (r.ray.direction * distance);
}
Ray CollisionSystem::getRayFromMouse(const QPoint &mousePos, const QRect &rect)
{
    vec3 mousePoint{static_cast<GLfloat>(mousePos.x()), static_cast<GLfloat>(mousePos.y()), 0.0f};
    auto inputSystem{registry->system<InputSystem>()};
    CameraController *curController{inputSystem->currentCameraController().get()};
    vec3 direction{curController->getCamera().calculateMouseRay(mousePoint, rect.height(), rect.width())};
    vec3 origin{curController->cameraPosition()};
    return Ray{origin, direction};
}
inline bool CollisionSystem::bothStatic(const Collision &lhs, const Collision &rhs)
{
    return (lhs.isStatic == rhs.isStatic) == true;
}

gsl::Vector3D CollisionSystem::getMin(const AABB &aabb)
{
    vec3 origin{aabb.transform.modelMatrix.getPosition()};
    vec3 p1{origin + aabb.size};
    vec3 p2{origin - aabb.size};

    return vec3{fminf(p1.x, p2.x),
                fminf(p1.y, p2.y),
                fminf(p1.z, p2.z)};
}

gsl::Vector3D CollisionSystem::getMax(const AABB &aabb)
{
    vec3 origin{aabb.transform.modelMatrix.getPosition()};
    vec3 p1{origin + aabb.size};
    vec3 p2{origin - aabb.size};

    return vec3{fmaxf(p1.x, p2.x),
                fmaxf(p1.y, p2.y),
                fmaxf(p1.z, p2.z)};
}
bool CollisionSystem::SphereSphere(const Sphere &sphere1, const Sphere &sphere2)
{
    vec3 sphere1Pos{sphere1.transform.modelMatrix.getPosition()};
    vec3 sphere2Pos{sphere2.transform.modelMatrix.getPosition()};
    // sum of radius
    float rs{sphere1.radius + sphere2.radius};
    // dist squared
    float dist{(sphere1Pos + sphere2Pos).length()};
    // compare
    return dist < (rs * rs);
}
bool CollisionSystem::SphereAABB(const Sphere &sphere, const AABB &aabb)
{
    // Get the actual position of the sphere - sphere.position only holds the offset from the entity it belongs to
    vec3 spherePos{sphere.transform.modelMatrix.getPosition()};
    // Not sure what ClosestPoint aims to achieve, previously I think it was sending a vec3(sphere.radius, 0,0) due to how vector3d works
    //    vec3 closestPoint = ClosestPoint(aabb, sphere.radius);
    // Now sends the center position of the sphere
    vec3 closestPoint{ClosestPoint(aabb, spherePos)};
    // not 100% sure about this one
    float dist{(spherePos - closestPoint).length()};
    float radiusSq{sphere.radius * sphere.radius};

    return dist < radiusSq;
}

bool CollisionSystem::AABBAABB(const AABB &AABB1, const AABB &AABB2)
{
    vec3 aMin{getMin(AABB1)};
    vec3 aMax{getMax(AABB1)};

    vec3 bMin{getMin(AABB2)};
    vec3 bMax{getMax(AABB2)};

    return aMin <= bMax && aMax >= bMin;
}

gsl::Vector3D CollisionSystem::ClosestPoint(const AABB &aabb, const vec3 &point)
{
    vec3 result{point};
    vec3 min{getMin(aabb)};
    vec3 max{getMax(aabb)};

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

gsl::Vector3D CollisionSystem::ClosestPoint(const Sphere &sphere, const vec3 &point)
{
    vec3 spherePos{sphere.transform.modelMatrix.getPosition()};
    vec3 sphereCenterToPoint{point - spherePos};
    sphereCenterToPoint.normalize();
    sphereCenterToPoint = sphereCenterToPoint * sphere.radius;

    return sphereCenterToPoint + spherePos;
}
bool CollisionSystem::calcRayToSphere(Raycast &r, const Sphere &sphere)
{
    vec3 center{sphere.transform.modelMatrix.getPosition() + sphere.position};
    vec3 originToCenter{r.ray.origin - center};

    float a{vec3::dot(r.ray.direction, r.ray.direction)};
    float b{2.f * vec3::dot(originToCenter, r.ray.direction)};
    float c{vec3::dot(originToCenter, originToCenter) - (sphere.radius * sphere.radius)};
    float discriminant{(b * b) - (4 * a * c)};
    if (discriminant < 0)
        return false;
    if (discriminant > 0.0f) {
        // get the 2 intersection distances along ray
        float sqrtDisc{sqrtf(discriminant)};
        double t_a{(-b + sqrtDisc) / (2 * a)};
        double t_b{(-b - sqrtDisc) / (2 * a)};
        r.intersectionDistance = t_b;
        // if behind viewer, throw one or both away
        if (t_a < 0.0f) {
            if (t_b < 0.0f)
                return false;
        }
        else if (t_b < 0.0f)
            r.intersectionDistance = t_a;
        return true;
    }
    // check for ray hitting once (skimming the surface)
    if (0.0f == discriminant) {
        // if behind viewer, throw away
        double t = {-b + sqrt(discriminant) / (2.0 * a)};
        if (t < 0.0f)
            return false;
        r.intersectionDistance = t;
        return true;
    }
    return false;
}

bool CollisionSystem::calcRayToAABB(Raycast &r, const AABB &aabb)
{
    vec3 AABBmin{getMin(aabb)};
    vec3 AABBmax{getMax(aabb)};

    float t1{(AABBmin.x - r.ray.origin.x) * r.ray.invDir.x};
    float t2{(AABBmax.x - r.ray.origin.x) * r.ray.invDir.x};

    float tmin{std::min(t1, t2)};
    float tmax{std::max(t1, t2)};

    for (int i{1}; i < 3; ++i) {
        t1 = (AABBmin[i] - r.ray.origin[i]) * r.ray.invDir[i];
        t2 = (AABBmax[i] - r.ray.origin[i]) * r.ray.invDir[i];

        tmin = std::max(tmin, std::min(t1, t2));
        tmax = std::min(tmax, std::max(t1, t2));
    }

    if (tmax >= std::max(tmin, 0.f)) {
        r.intersectionDistance = tmax; // This might need to be tmin instead
        return true;
    }
    return false;
}
// ************* Collider update slots *************
void CollisionSystem::setOriginX(double xIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &aabb{registry->get<AABB>(entityID)};
    aabb.origin.x = xIn;
    aabb.transform.matrixOutdated = true;
}
void CollisionSystem::setOriginY(double yIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &aabb{registry->get<AABB>(entityID)};
    aabb.origin.y = yIn;
    aabb.transform.matrixOutdated = true;
}
void CollisionSystem::setOriginZ(double zIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &aabb{registry->get<AABB>(entityID)};
    aabb.origin.z = zIn;
    aabb.transform.matrixOutdated = true;
}
void CollisionSystem::setAABBSizeX(double xIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &aabb{registry->get<AABB>(entityID)};
    aabb.size.x = xIn;
    aabb.transform.matrixOutdated = true;
}
void CollisionSystem::setAABBSizeY(double yIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &aabb{registry->get<AABB>(entityID)};
    aabb.size.y = yIn;
    aabb.transform.matrixOutdated = true;
}
void CollisionSystem::setAABBSizeZ(double zIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &aabb{registry->get<AABB>(entityID)};
    aabb.size.z = zIn;
    aabb.transform.matrixOutdated = true;
}
void CollisionSystem::setSpherePositionX(double xIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &sphere{registry->get<Sphere>(entityID)};
    sphere.position.x = xIn;
    sphere.transform.matrixOutdated = true;
}
void CollisionSystem::setSpherePositionY(double yIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &sphere{registry->get<Sphere>(entityID)};
    sphere.position.y = yIn;
    sphere.transform.matrixOutdated = true;
}
void CollisionSystem::setSpherePositionZ(double zIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &sphere{registry->get<Sphere>(entityID)};
    sphere.position.z = zIn;
    sphere.transform.matrixOutdated = true;
}
void CollisionSystem::setSphereRadius(double radius)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &sphere{registry->get<Sphere>(entityID)};
    sphere.radius = radius;
    sphere.transform.matrixOutdated = true;
}
void CollisionSystem::setObjectType(int index)
{
    bool isStatic;
    if (index == 0)
        isStatic = true;
    else
        isStatic = false;
    GLuint entityID{registry->getSelectedEntity()};
    if (registry->contains<AABB>(entityID)) {
        auto &aabb{registry->get<AABB>(entityID)};
        aabb.isStatic = isStatic;
        return;
    }
    if (registry->contains<Sphere>(entityID)) {
        auto &sphere{registry->get<Sphere>(entityID)};
        sphere.isStatic = isStatic;
    }
}
