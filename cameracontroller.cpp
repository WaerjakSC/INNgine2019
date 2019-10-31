#include "cameracontroller.h"

CameraController::CameraController(float aspectRatio)
    : mAspectRatio(aspectRatio),
      mCamera(Camera(mFieldOfView, mAspectRatio, mNearPlane, mFarPlane)) {
}
void CameraController::pitch(float degrees) {
    //  rotate around mRight
    mPitch -= degrees;
    updateForwardVector();
}
void CameraController::yaw(float degrees) {
    // rotate around mUp
    mYaw -= degrees;
    updateForwardVector();
}
void CameraController::setPitch(float newPitch) {
    mPitch = newPitch;
    updateForwardVector();
}
void CameraController::setYaw(float newYaw) {
    mPitch = newYaw;
    updateForwardVector();
}
vec3 CameraController::up() const {
    return mUp;
}

vec3 CameraController::forward() const {
    return mForward;
}
vec3 CameraController::cameraPosition() const {
    return mCameraPosition;
}
void CameraController::updateRightVector() {
    mRight = mForward ^ mUp;
    mRight.normalize();
}

void CameraController::updateForwardVector() {
    mRight = vec3(1.f, 0.f, 0.f);
    mRight.rotateY(mYaw);
    mRight.normalize();
    mUp = vec3(0.f, 1.f, 0.f);
    mUp.rotateX(mPitch);
    mUp.normalize();
    mForward = mUp ^ mRight;
    mForward.normalize();

    updateRightVector();
}
void CameraController::setPosition(const vec3 &position) {
    mCameraPosition = position;
}
void CameraController::update() {
    mCamera.setRotation(mPitch, mYaw);
    mCamera.setPosition(mCameraPosition);
    mCamera.calculateViewMatrix();
}
void CameraController::setSpeed(float speed) {
    mTranslationSpeed = speed;
}

/**
 * @brief go to location
 * @param target
 */
void CameraController::goTo(vec3 target) {
    vec3 targetDistance{0, 0, 5};
    const vec3 position = target + targetDistance;
    const vec3 direction = (position - target).normalized();

    mYaw = gsl::rad2degf(gsl::atan2(direction.x, direction.z));
    mPitch = gsl::rad2degf(gsl::asin(-direction.y));
    mCameraPosition = position;
    updateForwardVector();
}
void CameraController::moveForward(float delta) {
    mCameraPosition += mForward * mTranslationSpeed * delta;
}
void CameraController::moveUp(float deltaHeight) {
    mCameraPosition.y += mTranslationSpeed * deltaHeight;
}
void CameraController::moveRight(float delta) {
    //This fixes a bug in the up and right calculations
    //so camera always holds its height when strafing
    //should be fixed through correct right calculations!
    vec3 right = mRight;
    right.y = 0.f;
    mCameraPosition += right * mTranslationSpeed * delta;
}
void CameraController::resize(float aspectRatio) {
    mAspectRatio = aspectRatio;
    mCamera.setProjectionMatrix(mFieldOfView, mAspectRatio, mNearPlane, mFarPlane);
}

GameCameraController::GameCameraController(float aspectRatio) : CameraController(aspectRatio) {
    mPitch = -45.f;
    // Set desired pitch and yaw here, they will not be changeable by the player
}

void GameCameraController::pitch(float degrees) {
    Q_UNUSED(degrees);
}

void GameCameraController::yaw(float degrees) {
    Q_UNUSED(degrees);
}
