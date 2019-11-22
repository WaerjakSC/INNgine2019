#include "cameracontroller.h"
#include "movementsystem.h"
#include "registry.h"
CameraController::CameraController(float aspectRatio)
    : mAspectRatio(aspectRatio),
      mCamera(Camera(mFieldOfView, mAspectRatio, mNearPlane, mFarPlane)) {
}
void CameraController::pitch(float degrees) {
    mOutDated = true;

    //  rotate around mRight
    mPitch -= degrees;
    updateForwardVector();
}
void CameraController::yaw(float degrees) {
    mOutDated = true;

    // rotate around mUp
    mYaw -= degrees;
    updateForwardVector();
}
void CameraController::setPitch(float newPitch) {
    mOutDated = true;
    mPitch = newPitch;
    updateForwardVector();
}
void CameraController::setYaw(float newYaw) {
    mOutDated = true;
    mYaw = newYaw;
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
    mOutDated = true;
    mCameraPosition = position;
}
void CameraController::update() {
    if (mOutDated) {
        mCamera.setRotation(mPitch, mYaw);
        mCamera.setPosition(mCameraPosition);
        mCamera.calculateViewMatrix();
        mOutDated = false;
    }
}
void CameraController::setSpeed(float speed) {
    mTranslationSpeed = speed;
}

/**
 * @brief go to location
 * @param target
 */
void CameraController::goTo(vec3 target) {
    mOutDated = true;

    vec3 targetDistance{0, 0, 5};
    const vec3 position = target + targetDistance;
    const vec3 direction = (position - target).normalized();

    mYaw = gsl::rad2degf(gsl::atan2(direction.x, direction.z));
    mPitch = gsl::rad2degf(gsl::asin(-direction.y));
    mCameraPosition = position;
    updateForwardVector();
}
float CameraController::getPitch() const {
    return mPitch;
}

float CameraController::getYaw() const {
    return mYaw;
}
const vec3 CameraController::getCameraRotation() const {
    return mCamera.getRotation();
}

void CameraController::moveForward(float delta) {
    mOutDated = true;
    mCameraPosition += mForward * mTranslationSpeed * delta;
}
void CameraController::moveRight(float delta) {
    mOutDated = true;
    //This fixes a bug in the up and right calculations
    //so camera always holds its height when strafing
    //should be fixed through correct right calculations!
    vec3 right = mRight;
    right.y = 0.f;
    mCameraPosition += right * mTranslationSpeed * delta;
}
void CameraController::moveUp(float deltaHeight) {
    mOutDated = true;
    mCameraPosition.y += mTranslationSpeed * deltaHeight;
}

void CameraController::resize(float aspectRatio) {
    mOutDated = true;
    mAspectRatio = aspectRatio;
    mCamera.setProjectionMatrix(mFieldOfView, mAspectRatio, mNearPlane, mFarPlane);
}
vec3 GameCameraController::cameraPosition() const {
    return mGameCam.mCameraPosition;
}
GameCameraController::GameCameraController(float aspectRatio, GameCamera &gameCam, GLuint controller)
    : CameraController(aspectRatio), mGameCam(gameCam), mControllerID(controller) {
    mPitch = gameCam.mPitch;
    mYaw = gameCam.mYaw;
    setPosition(gameCam.mCameraPosition);
    mTranslationSpeed = 9.5f;
}

void GameCameraController::pitch(float degrees) {
    Q_UNUSED(degrees);
}

void GameCameraController::yaw(float degrees) {
    Q_UNUSED(degrees);
}

void GameCameraController::update() {
    if (mGameCam.mOutDated) {
        //        updateMeshPosition();
        mCamera.setRotation(mGameCam.mPitch, mGameCam.mYaw);
        mCamera.setPosition(mGameCam.mCameraPosition);
        mCamera.calculateViewMatrix();
        mGameCam.mOutDated = false;
    }
}

bool GameCameraController::isActive() {
    return mGameCam.mIsActive;
}

void GameCameraController::setPosition(const vec3 &position) {
    CameraController::setPosition(position);
}
void GameCameraController::moveForward(float delta) {
    mGameCam.mOutDated = true;
    mGameCam.mCameraPosition += mForward * mTranslationSpeed * delta;
}
void GameCameraController::moveRight(float delta) {
    mGameCam.mOutDated = true;
    //This fixes a bug in the up and right calculations
    //so camera always holds its height when strafing
    //should be fixed through correct right calculations!
    vec3 right = mRight;
    right.y = 0.f;
    mGameCam.mCameraPosition += right * mTranslationSpeed * delta;
}
void GameCameraController::updateMeshPosition() {
    gsl::Matrix4x4 temp(true);
    temp.lookAt(cameraPosition(), positionWithOffset(), up());
    auto [pos, sca, rot] = gsl::Matrix4x4::decomposed(temp);
    Q_UNUSED(pos);
    Q_UNUSED(sca);
    auto moveSys = Registry::instance()->getSystem<MovementSystem>();
    moveSys->setLocalPosition(mControllerID, positionWithOffset());
    moveSys->setRotation(mControllerID, rot);
}
vec3 GameCameraController::positionWithOffset() {
    return cameraPosition() - forward();
}

GLuint GameCameraController::controllerID() const {
    return mControllerID;
}
