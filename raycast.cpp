#include "raycast.h"
#include "camera.h"
#include "collisionsystem.h"
#include "movementsystem.h"
#include "registry.h"
#include "renderwindow.h"
#include <QWindow>
Raycast::Raycast(cjk::Ref<CameraController> controller, float range) : mCurrentController(controller), rayRange(range) {
}
/**
 * @brief Raycast::rayCast
 * @param mousePos
 * @return For now this returns -1 if it doesn't hit any targets. Make sure to check if it's valid a valid entity before using it
 */
int Raycast::mousePick(const QPoint &mousePos, const QRect &rect) {
    Ray ray = getRayFromMouse(mousePos, rect);

    int entityID{-1};
    double closestTarget{rayRange};
    entityID = checkAABB(ray, closestTarget);
    int sphereID = checkSphere(ray, closestTarget);
    if (sphereID != -1) {
        entityID = sphereID;
    }
    return entityID;
}
int Raycast::mousePick(const QPoint &mousePos, const QRect &rect, vec3 &hitPoint, int ignoredEntity) {
    Ray ray = getRayFromMouse(mousePos, rect);

    int entityID{-1};
    double closestTarget{rayRange};

    entityID = checkAABB(ray, closestTarget, ignoredEntity);
    hitPoint = getPointOnRay(ray, closestTarget);
    int sphereID = checkSphere(ray, closestTarget, ignoredEntity);
    if (sphereID != -1) {
        entityID = sphereID;
        hitPoint = getPointOnRay(ray, closestTarget);
    }
    if (entityID == -1)
        hitPoint = getPointOnRay(ray, rayRange);
    return entityID;
}
int Raycast::checkAABB(const Ray &ray, double &closestTarget, int ignoredEntity) {
    Registry *registry = Registry::instance();
    auto collisionSystem = registry->getSystem<CollisionSystem>();
    double intersectionPoint;
    int entityID{-1};
    auto view = registry->view<AABB>();
    for (auto entity : view) {
        if ((int)entity == ignoredEntity)
            continue;
        auto &aabb = view.get(entity);
        if (collisionSystem->RayToAABB(ray, aabb, intersectionPoint)) {
            if (intersectionPoint < closestTarget) {
                closestTarget = intersectionPoint;
                entityID = entity;
            }
        }
    }
    return entityID;
}
int Raycast::checkSphere(const Ray &ray, double &closestTarget, int ignoredEntity) {
    Registry *registry = Registry::instance();
    auto collisionSystem = registry->getSystem<CollisionSystem>();
    double intersectionPoint;
    int entityID{-1};
    auto view = registry->view<Sphere>();
    for (auto entity : view) {
        if ((int)entity == ignoredEntity)
            continue;
        auto &sphere = view.get(entity);
        if (collisionSystem->RayToSphere(ray, sphere, intersectionPoint)) {
            if (intersectionPoint < closestTarget) {
                closestTarget = intersectionPoint;
                entityID = entity;
            }
        }
    }
    return entityID;
}
vec3 Raycast::getPointOnRay(const Ray &ray, float distance) {
    return ray.origin + (ray.direction * distance);
}
Ray Raycast::getRayFromMouse(const QPoint &mousePos, const QRect &rect) {
    vec3 mousePoint(mousePos.x(), mousePos.y(), 0.0f);
    vec3 direction = mCurrentController->getCamera().calculateMouseRay(mousePoint, rect.height(), rect.width());
    vec3 origin = mCurrentController->cameraPosition();

    return Ray{origin, direction};
}

// Remove this?
//bool Raycast::RayToPlane(Entity *entt, float start, float finish, const Ray &ray) {
//    vec3 startPoint = getPointOnRay(ray, start);
//    vec3 endPoint = getPointOnRay(ray, finish);
//}
