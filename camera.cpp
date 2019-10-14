#include "camera.h"
#include "innpch.h"

Camera::Camera() {
    mViewMatrix.setToIdentity();
    mProjectionMatrix.setToIdentity();

    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();
}

void Camera::pitch(float degrees) {
    //  rotate around mRight
    mPitch -= degrees;
    updateForwardVector();
}
void Camera::setPitch(float newPitch) {
    mPitch = newPitch;
    updateForwardVector();
}
void Camera::setYaw(float newYaw) {
    mPitch = newYaw;
    updateForwardVector();
}
/**
 * @brief go to location
 * @param target
 */
void Camera::goTo(gsl::Vector3D target) {
    gsl::Vector3D targetDistance{0, 0, 5};
    const gsl::Vector3D position = target + targetDistance;
    const gsl::Vector3D direction = (position - target).normalized();

    mYaw = gsl::rad2degf(gsl::atan2(direction.x, direction.z));
    mPitch = gsl::rad2degf(gsl::asin(-direction.y));
    mPosition = position;
    updateForwardVector();
}
void Camera::yaw(float degrees) {
    // rotate around mUp
    mYaw -= degrees;
    updateForwardVector();
}

void Camera::updateRightVector() {
    mRight = mForward ^ mUp;
    mRight.normalize();
    //    qDebug() << "Right " << mRight;
}

void Camera::updateForwardVector() {
    mRight = gsl::Vector3D(1.f, 0.f, 0.f);
    mRight.rotateY(mYaw);
    mRight.normalize();
    mUp = gsl::Vector3D(0.f, 1.f, 0.f);
    mUp.rotateX(mPitch);
    mUp.normalize();
    mForward = mUp ^ mRight;

    updateRightVector();
}

void Camera::update() {
    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();

    mPitchMatrix.rotateX(-mPitch);
    mYawMatrix.rotateY(-mYaw);

    mPosition -= mForward * mSpeed;

    mViewMatrix = mPitchMatrix * mYawMatrix;
    mViewMatrix.translate(-mPosition);
}

void Camera::setPosition(const gsl::Vector3D &position) {
    mPosition = position;
}

void Camera::setSpeed(float speed) {
    mSpeed = speed;
}

void Camera::updateHeight(float deltaHeight) {
    mPosition.y += deltaHeight;
}

void Camera::moveRight(float delta) {
    //This fixes a bug in the up and right calculations
    //so camera always holds its height when straifing
    //should be fixed thru correct right calculations!
    gsl::Vector3D right = mRight;
    right.y = 0.f;
    mPosition += right * delta;
}

gsl::Vector3D Camera::position() const {
    return mPosition;
}

gsl::Vector3D Camera::up() const {
    return mUp;
}

gsl::Vector3D Camera::forward() const {
    return mForward;
}
vec3 Camera::calculateMouseRay(const vec3 &viewportPoint, int height, int width) {
    vec3 rayNDC = getNormalizedDeviceCoords(viewportPoint, height, width);

    vec4 rayClip = vec4(rayNDC.x, rayNDC.y, -1.0, 1.0);

    gsl::Matrix4x4 projMatrix = mProjectionMatrix;
    projMatrix.inverse();

    vec4 rayEye = projMatrix * rayClip; // Invert the perspective matrix to get to view/eye space (camera)
    rayEye = vec4(rayEye.x, rayEye.y, -1.0, 0.0);

    gsl::Matrix4x4 viewMatrix = mViewMatrix;
    viewMatrix.inverse();
    vec3 rayWorld = (viewMatrix * rayEye).getXYZ();
    rayWorld.normalize();

    return rayWorld;
}
vec3 Camera::getNormalizedDeviceCoords(const vec3 &mouse, int height, int width) {
    float x = (2.0f * mouse.x) / width - 1.0f;
    float y = 1.0f - (2.0f * mouse.y) / height;
    float z = mouse.z;
    return vec3(x, y, z); // Normalised Device Coordinates range [-1:1, -1:1, -1:1]
}
