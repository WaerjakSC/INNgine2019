#include "raycast.h"
#include "camera.h"
#include "collisionsystem.h"
#include "movementsystem.h"
#include "registry.h"
#include "renderwindow.h"
#include <QWindow>
Raycast::Raycast(cjk::Ref<CameraController> controller) : mCurrentController(controller) {
}
/**
 * @brief Raycast::rayCast
 * @param mousePos
 * @return For now this returns -1 if it doesn't hit any targets. Make sure to check if it's valid a valid entity before using it
 */
int Raycast::mousePick(const QPoint &mousePos, const QRect &rect) {
    Registry *registry = Registry::instance();
    Ray ray = getRayFromMouse(mousePos, rect);

    int entityID{-1};
    double closestTarget{rayRange};
    double intersectionPoint;
    Ref<CollisionSystem> collisionSystem = registry->getSystem<CollisionSystem>();
    auto view = registry->view<AABB>();
    for (auto entity : view) {
        auto &aabb = view.get(entity);
        if (collisionSystem->RayToAABB(ray, aabb, intersectionPoint)) {
            if (intersectionPoint < closestTarget) {
                closestTarget = intersectionPoint;
                entityID = entity;
            }
        }
    }
    auto sphereView = registry->view<Sphere>();
    for (auto entity : sphereView) {
        auto &sphere = sphereView.get(entity);
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
