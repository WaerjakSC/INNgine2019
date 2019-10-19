#include "raycast.h"
#include "camera.h"
#include "registry.h"
#include "renderwindow.h"
#include <QWindow>
Raycast::Raycast(RenderWindow *window, Camera *camera) : mOpenGLWindow(window), mCurrentCamera(camera) {
}
/**
 * @brief Raycast::rayCast
 * @param mousePos
 * @return For now this returns -1 if it doesn't hit any targets. Make sure to check if it's valid a valid entity before using it
 */
int Raycast::rayCast(const QPoint &mousePos) {
    Registry *registry = Registry::instance();
    Ray ray = getRay(mousePos);
    // actual point in world space
    //    vec3 rayPoint = getPointOnRay(ray, 5);
    int entityID{-1};
    double closestTarget{rayRange};
    for (auto entity : registry->getEntities()) {
        if (entity.second->name() != "Skybox" && entity.second->name() != "XYZ") {
            double intersectionPoint;
            const Transform &trans = registry->getComponent<Transform>(entity.second->id());
            if (RayToSphere(ray, trans.position, 1, intersectionPoint)) { // Setting radius to 1 just for testing
                if (intersectionPoint < closestTarget) {
                    closestTarget = intersectionPoint;
                    entityID = entity.second->id();
                }
            }
        }
    }
    return entityID;
}

vec3 Raycast::getPointOnRay(const Ray &ray, float distance) {
    return ray.origin + (ray.direction * distance);
}
Ray Raycast::getRay(const QPoint &mousePos) {
    const QRect &rect = mOpenGLWindow->geometry();

    vec3 mousePoint(mousePos.x(), mousePos.y(), 0.0f);
    vec3 direction = mCurrentCamera->calculateMouseRay(mousePoint, rect.height(), rect.width());
    vec3 origin = mCurrentCamera->position();

    return Ray{origin, direction};
}

bool Raycast::RayToSphere(const Ray &ray, const vec3 &center, double radius, double &intersectionDistance) {
    vec3 originToCenter = ray.origin - center;

    float a = vec3::dot(ray.direction, ray.direction);
    float b = 2.0 * vec3::dot(originToCenter, ray.direction);
    float c = vec3::dot(originToCenter, originToCenter) - (radius * radius);
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
// Remove this?
//bool Raycast::RayToPlane(Entity *entt, float start, float finish, const Ray &ray) {
//    vec3 startPoint = getPointOnRay(ray, start);
//    vec3 endPoint = getPointOnRay(ray, finish);
//}
