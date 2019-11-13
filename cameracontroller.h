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
    virtual void update();

    void setSpeed(float speed);
    void moveUp(float deltaHeight);
    void moveRight(float delta);
    virtual void setPosition(const vec3 &position);
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

    float getPitch() const;

    float getYaw() const;
    const vec3 getCameraRotation() const;

private:
    float mNearPlane{0.5f};
    float mFarPlane{500.f};
    float mFieldOfView{45.f};
    float mAspectRatio{1.7778f};

    vec3 mForward{0.f, 0.f, -1.f};
    vec3 mRight{1.f, 0.f, 0.f};
    vec3 mUp{0.f, 1.f, 0.f};
    vec3 mCameraPosition{0.f, 0.f, 0.f};

    float mTranslationSpeed{0.02f}; //camera will move by this speed along the mForward vector
    float mRotationSpeed{180.f};

protected:
    Camera mCamera;

    float mPitch{-25.f};
    float mYaw{0.f};
    bool mOutDated{true};
};
class GameCameraController : public CameraController {
public:
    GameCameraController(float aspectRatio, GameCamera &gameCam, GLuint controller);

    void pitch(float degrees) override;
    void yaw(float degrees) override;
    void update() override;
    bool isActive();
    /**
     * @brief setPosition overloaded function used by movementsystem to set position of the mesh representation of the game camera.
     * @param position
     */
    void setPosition(const vec3 &position) override;
    void updateMeshPosition();

    vec3 positionWithOffset();

    GLuint controllerID() const;

private:
    GameCamera &mGameCam;
    GLuint mControllerID;
};

#endif // CAMERACONTROLLER_H
