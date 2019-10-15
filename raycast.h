#ifndef RAYCAST_H
#define RAYCAST_H
#include "gsl_math.h"

#include <QPoint>
struct Ray {
    gsl::Vector3D origin;
    gsl::Vector3D direction;
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
    gsl::Vector3D getPointOnRay(const Ray &ray, float distance);
    Ray getRay(const QPoint &mousePos);
    bool RayToSphere(const Ray &ray, const gsl::Vector3D &center, double sphereRadius, double &intersectionDistance);
    //    bool RayToPlane(Entity *entt, float start, float finish, const Ray &ray);
    float rayRange{250};
};

#endif // RAYCAST_H
