#ifndef CAMERA_H
#define CAMERA_H

#include "components.h"
#include "innpch.h"
typedef gsl::Vector3D vec3;
typedef gsl::Vector4D vec4;
typedef gsl::Matrix4x4 mat4;
class Camera {
public:
    Camera() {}
    Camera(float fov, float aspectRatio, float near, float far);

    gsl::Matrix4x4 mViewMatrix;
    gsl::Matrix4x4 mProjectionMatrix;

    void setPosition(const vec3 &position);

    void setSpeed(float speed);
    void updateHeight(float deltaHeigth);
    void moveRight(float delta);

    vec3 position() const;

    vec3 getNormalizedDeviceCoords(const vec3 &viewportPoint, int height, int width);
    vec3 calculateMouseRay(const vec3 &viewportPoint, int height, int width);

    gsl::Matrix4x4 getViewMatrix() const;
    gsl::Matrix4x4 getProjectionMatrix() const;

    /**
      * @brief Frustum struct
      */

    //    typedef struct Frustum {

    //        struct {
    //            Plane top;
    //            Plane bottom;
    //            Plane left;
    //            Plane right;
    //            Plane near;
    //            Plane far;
    //        } planeType;

    //        Plane planes[6];

    //        inline Frustum() {}
    //        vec3 Intersection(Plane p1, Plane p2, Plane p3);
    //        void GetCorners(const Frustum &f, vec3 *outCorners);
    //        bool Intersects(const Frustum &f, const Sphere &s);
    //        float Classify(const AABB &aabb, const Plane &plane);
    //        //        float Classify(const OBB &obb, const Plane &plane);
    //        bool Intersects(const Frustum &f, const AABB &aabb);
    //        //        bool Intersects(const Frustum &f, const OBB &obb);

    //    } Frustum;
    //    Frustum getFrustum() const {
    //        return mFrustum;
    //    }
    //    void makeFrustum();

    void setProjectionMatrix(float fov, float aspect, float nearPlane = 0.5f, float farPlane = 200.f);
    void setProjectionMatrix();

    void setRotation(float pitch, float yaw);

    const vec3 getRotation() const;

private:
    vec3 mPosition{0.f, 0.f, 0.f};
    float mPitch, mYaw;

    gsl::Matrix4x4 mYawMatrix;
    gsl::Matrix4x4 mPitchMatrix;

    //    Frustum mFrustum;

    friend class CameraController;
    friend class GameCameraController;
    void calculateViewMatrix();
};

#endif // CAMERA_H
