#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H
#include "camera.h"
#include "core.h"
using namespace cjk;
class CameraController {
public:
    CameraController(float aspectRatio);

    void updateRightVector();
    void updateForwardVector();
    void update();

    void setSpeed(float speed);
    void moveUp(float deltaHeight);
    void moveRight(float delta);
    void setPosition(const vec3 &position);
    vec3 cameraPosition() const;

    virtual void pitch(float degrees);
    virtual void yaw(float degrees);
    void setPitch(float newPitch);
    void setYaw(float newYaw);

    void goTo(vec3 target);

    vec3 up() const;
    vec3 forward() const;

    Camera &getCamera() { return mCamera; }
    const Camera &getCamera() const { return mCamera; }
    void resize(float aspectRatio);

    void moveForward(float dt);

private:
    float mNearPlane{0.5f};
    float mFarPlane{500.f};
    float mFieldOfView{45.f};
    float mAspectRatio{1.7778f};

    Camera mCamera;

    vec3 mForward{0.f, 0.f, -1.f};
    vec3 mRight{1.f, 0.f, 0.f};
    vec3 mUp{0.f, 1.f, 0.f};
    vec3 mCameraPosition{0.f, 0.f, 0.f};

    float mTranslationSpeed{0.02f}; //camera will move by this speed along the mForward vector
    float mRotationSpeed{180.f};

protected:
    float mPitch{-25.f};
    float mYaw{0.f};
};
class GameCameraController : public CameraController {
public:
    GameCameraController(float aspectRatio);
    void pitch(float degrees) override;
    void yaw(float degrees) override;
};

#endif // CAMERACONTROLLER_H
