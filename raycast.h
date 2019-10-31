#ifndef RAYCAST_H
#define RAYCAST_H
#include "core.h"
#include "gsl_math.h"
#include <QPoint>
typedef gsl::Vector3D vec3;
struct Ray {
    vec3 origin;
    vec3 direction;
};
class CameraController;
class RenderWindow;
class Entity;
class Raycast {
public:
    Raycast(cjk::Ref<CameraController> controller);
    int mousePick(const QPoint &mousePos, const QRect &rect);

private:
    cjk::Ref<CameraController> mCurrentController;
    Ray ray;
    vec3 getPointOnRay(const Ray &ray, float distance);
    Ray getRayFromMouse(const QPoint &mousePos, const QRect &rect);
    bool RayToSphere(const Ray &ray, const vec3 &center, double sphereRadius, double &intersectionDistance);
    //    bool RayToPlane(Entity *entt, float start, float finish, const Ray &ray);
    float rayRange{250};
};

#endif // RAYCAST_H
