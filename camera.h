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

    void setPosition(const gsl::Vector3D &position);

    void setSpeed(float speed);
    void updateHeight(float deltaHeigth);
    void moveRight(float delta);

    gsl::Vector3D position() const;
    gsl::Vector3D up() const;

    gsl::Vector3D forward() const;

    void setPitch(float newPitch);
    void setYaw(float newYaw);
    void goTo(gsl::Vector3D target);    

    vec3 getNormalizedDeviceCoords(const vec3 &viewportPoint, int height, int width);
    vec3 calculateMouseRay(const vec3 &viewportPoint, int height, int width);

    Frustum getFrustum();

    gsl::Matrix4x4 getViewMatrix() const;
    gsl::Matrix4x4 getProjectionMatrix() const;

private:
    gsl::Vector3D mForward{0.f, 0.f, -1.f};
    gsl::Vector3D mRight{1.f, 0.f, 0.f};
    gsl::Vector3D mUp{0.f, 1.f, 0.f};

    gsl::Vector3D mPosition{0.f, 0.f, 0.f};
    float mPitch{0.f};
    float mYaw{0.f};

    gsl::Matrix4x4 mYawMatrix;
    gsl::Matrix4x4 mPitchMatrix;

    float mSpeed{0.f}; //camera will move by this speed along the mForward vector
};

#endif // CAMERA_H
