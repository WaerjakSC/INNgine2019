#ifndef CAMERA_H
#define CAMERA_H

#include "innpch.h"
#include "components.h"
typedef gsl::Vector3D vec3;
typedef gsl::Vector4D vec4;
typedef gsl::Matrix4x4 mat4;
class Camera {
public:
    Camera();

    void pitch(float degrees);
    void yaw(float degrees);
    void updateRightVector();
    void updateForwardVector();
    void update();

    gsl::Matrix4x4 mViewMatrix;
    gsl::Matrix4x4 mProjectionMatrix;

    void setPosition(const vec3 &position);

    void setSpeed(float speed);
    void updateHeight(float deltaHeigth);
    void moveRight(float delta);

    vec3 position() const;
    vec3 up() const;

    vec3 forward() const;

    void setPitch(float newPitch);
    void setYaw(float newYaw);
    void goTo(vec3 target);

    vec3 getNormalizedDeviceCoords(const vec3 &viewportPoint, int height, int width);
    vec3 calculateMouseRay(const vec3 &viewportPoint, int height, int width);

    gsl::Matrix4x4 getViewMatrix() const;
    gsl::Matrix4x4 getProjectionMatrix() const;

    /**
      * @brief Frustum struct
      */
    typedef Collision::Plane plane;
    typedef Collision::Sphere sphere;
    typedef Collision::AABB aABB;
    typedef Collision::OBB oBB;
    typedef struct Frustum {
        union {
            struct {
                plane top;
                plane bottom;
                plane left;
                plane right;
                plane near;
                plane far;
            } planeType;
            plane planes[6];
        };
        inline Frustum() { }
        vec3 Intersection(plane p1, plane p2, plane p3);
        void GetCorners(const Frustum& f, vec3* outCorners);
        bool Intersects(const Frustum& f, const sphere& s);
        float Classify(const aABB& aabb, const plane& plane);
        float Classify(const oBB& obb, const plane& plane);
        bool Intersects(const Frustum& f, const aABB& aabb);
        bool Intersects(const Frustum& f, const oBB& obb);

    } Frustum;

    Frustum getFrustum();

private:
    vec3 mForward{0.f, 0.f, -1.f};
    vec3 mRight{1.f, 0.f, 0.f};
    vec3 mUp{0.f, 1.f, 0.f};

    vec3 mPosition{0.f, 0.f, 0.f};
    float mPitch{0.f};
    float mYaw{0.f};

    gsl::Matrix4x4 mYawMatrix;
    gsl::Matrix4x4 mPitchMatrix;

    float mSpeed{0.f}; //camera will move by this speed along the mForward vector
};

#endif // CAMERA_H
