#ifndef CAMERA_H
#define CAMERA_H

#include "components.h"
#include "innpch.h"
// must undefine for Frustum.
#undef near
#undef far
namespace cjk {
class Camera {
    using vec3 = gsl::Vector3D;
    using vec4 = gsl::Vector4D;
    using mat4 = gsl::Matrix4x4;
    using mat3 = gsl::Matrix3x3;

public:
    Camera() {}
    Camera(float fov, float aspectRatio, float near, float far);

    mat4 mViewMatrix;
    mat4 mProjectionMatrix;

    void setPosition(const vec3 &position);

    void setSpeed(float speed);
    void updateHeight(float deltaHeigth);
    void moveRight(float delta);

    vec3 position() const;

    vec3 getNormalizedDeviceCoords(const vec3 &viewportPoint, int height, int width);
    vec3 calculateMouseRay(const vec3 &viewportPoint, int height, int width);

    mat4 getViewMatrix() const;
    mat4 getProjectionMatrix() const;

    /**
    * @brief Plane struct for use with Frustum
    */
    struct Plane : public Collision {
        vec3 normal;
        float distance{0};
        // a plane is defined by a normal and distance from origo
        Plane(const vec3 &n = vec3{1, 0, 0}, bool stat = true) : normal(n)
        {
            isStatic = stat;
        }
    };

    /**
      * @brief Frustum struct
      */
    typedef struct Frustum {

        struct {
            Plane top;
            Plane bottom;
            Plane left;
            Plane right;
            Plane near;
            Plane far;
        } plane;

        Plane planes[6];

        inline Frustum() {}
        vec3 Intersection(Plane p1, Plane p2, Plane p3);
        void GetCorners(const Frustum &f, std::vector<vec3> &outCorners);
        float Classify(const AABB &aabb, const Plane &plane);
        bool Intersects(const Frustum &f, const Sphere &s);
        bool Intersects(const Frustum &f, const AABB &aabb);

    } Frustum;
    Frustum getFrustum() const
    {
        return mFrustum;
    }
    void makeFrustum();

    void setProjectionMatrix(float fov, float aspect, float nearPlane = 0.5f, float farPlane = 200.f);
    void setProjectionMatrix();

    void setRotation(float pitch, float yaw);

    const vec3 getRotation() const;

private:
    vec3 mPosition{0.f, 0.f, 0.f};
    float mPitch, mYaw;

    mat4 mYawMatrix;
    mat4 mPitchMatrix;

    Frustum mFrustum;

    friend class CameraController;
    friend class GameCameraController;
    void calculateViewMatrix();
};
} // namespace cjk

#endif // CAMERA_H
