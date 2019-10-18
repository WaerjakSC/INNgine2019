#ifndef RAYCAST_H
#define RAYCAST_H
#include "gsl_math.h"

#include <QPoint>
typedef gsl::Vector3D vec3;
struct Ray {
    vec3 origin;
    vec3 direction;
};
class Camera;
class RenderWindow;
class Entity;
class Raycast {
public:
    Raycast(RenderWindow *window, Camera *camera);
    int rayCast(const QPoint &mousePos);

private:
    RenderWindow *mOpenGLWindow;
    Camera *mCurrentCamera;
    Ray ray;
    vec3 getPointOnRay(const Ray &ray, float distance);
    Ray getRay(const QPoint &mousePos);
    bool RayToSphere(const Ray &ray, const vec3 &center, double sphereRadius, double &intersectionDistance);
    //    bool RayToPlane(Entity *entt, float start, float finish, const Ray &ray);
    float rayRange{250};
};

#endif // RAYCAST_H
