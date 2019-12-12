#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H
#include "camera.h"
#include "core.h"

class CameraController {
    using vec3 = gsl::Vector3D;

public:
    CameraController(float aspectRatio);

    /**
     * Get an updated right vector.
     */
    void updateRightVector();
    /**
     * Get an updated forward vector.
     */
    void updateForwardVector();
    /**
     * Update the camera if it's out of date.
     */
    virtual void update();

    /**
     * Set the movement speed of the editor camera.
     * @param speed
     */
    void setSpeed(float speed);

    void moveUp(float deltaHeight);
    virtual void moveRight(float delta);
    virtual void moveForward(float dt);

    virtual void setPosition(const vec3 &position);
    virtual vec3 cameraPosition() const;

    virtual void pitch(float degrees);
    virtual void yaw(float degrees);
    void setPitch(float newPitch);
    void setYaw(float newYaw);

    /**
     * Go to a specific point in the scene.
     * @param target
     */
    void goTo(vec3 target);

    vec3 up() const;
    vec3 forward() const;
    vec3 right() const;

    Camera &getCamera() { return mCamera; }
    const Camera &getCamera() const { return mCamera; }

    /**
     * Change the aspect ratio in the event of a resize event from renderwindow.
     * @param aspectRatio
     */
    void resize(float aspectRatio);

    float getPitch() const;

    float getYaw() const;
    const vec3 getCameraRotation() const;

protected:
    using mat4 = gsl::Matrix4x4;

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

    float mPitch{-50.f};
    float mYaw{0.f};
    bool mOutDated{true};
};
class GameCameraController : public CameraController {
    using vec3 = gsl::Vector3D;
    using mat4 = gsl::Matrix4x4;

public:
    GameCameraController(float aspectRatio, GLuint controller);

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

    void moveForward(float delta) override;
    void moveRight(float delta) override;

    vec3 cameraPosition() const override;

    void setupController();

private:
    GLuint mControllerID;
};

#endif // CAMERACONTROLLER_H
