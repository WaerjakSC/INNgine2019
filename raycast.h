#ifndef RAYCAST_H
#define RAYCAST_H
#include "core.h"
#include "gsl_math.h"
#include <QPoint>
typedef gsl::Vector3D vec3;
struct Ray {
    Ray() {}
    Ray(const vec3 &orig, const vec3 &dir) : origin(orig), direction(dir) {
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
class CameraController;
class RenderWindow;
class Entity;
class Raycast {
public:
    Raycast(cjk::Ref<CameraController> controller, float range = 250.f);
    int mousePick(const QPoint &mousePos, const QRect &rect, vec3 &hitPoint, int ignoredEntity = -1);
    int mousePick(const QPoint &mousePos, const QRect &rect);

private:
    cjk::Ref<CameraController> mCurrentController;
    vec3 getPointOnRay(const Ray &ray, float distance);
    Ray getRayFromMouse(const QPoint &mousePos, const QRect &rect);
    int checkAABB(const Ray &ray, double &closestTarget, int ignoredEntity = -1);
    int checkSphere(const Ray &ray, double &closestTarget, int ignoredEntity = -1);

    float rayRange{250};
};

#endif // RAYCAST_H
