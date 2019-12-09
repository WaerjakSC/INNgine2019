#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H
#include "core.h"
#include "isystem.h"
#include "pool.h"
class RenderWindow;

class Registry;
class ResourceManager;
class MovementSystem;
class CameraController;
class GameCameraController;
class InputSystem : public QObject, public ISystem {
    Q_OBJECT
    using vec3 = gsl::Vector3D;

public:
    InputSystem(RenderWindow *window);
    void update(DeltaTime dt = 0.016) override;
    void updatePlayOnly(DeltaTime deltaTime = 0.016);
    void init(float aspectRatio = 1.92f);
    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    Input playerController() const;
    Input &playerController();

    cjk::Ref<CameraController> editorCamController() const;
    void setEditorCamController(const cjk::Ref<CameraController> &editorCamController);

    std::vector<cjk::Ref<GameCameraController>> gameCameraControllers() const;
    cjk::Ref<CameraController> currentCameraController();

    void onResize(float aspectRatio);

    void setGameCameraInactive();

    void reset();
    void setBuildableDebug(bool value);

public slots:
    void setCameraPositionX(double xIn);
    void setCameraPositionY(double yIn);
    void setCameraPositionZ(double zIn);
    void setPlaneColors(bool state);

    void setActiveCamera(bool checked);

    void setPitch(double pitch);
    void setYaw(double yaw);
    void setBuildableObject(bool state);
signals:
    void rayHitEntity(GLuint entityID);
    void closeEngine();
    void toggleRendered(GLuint entityID);

private:
    /**
     * @brief handleKeyInput takes care of certain hard-coded key inputs
     */
    void handleKeyInput();
    /**
     * @brief handleMouseInput mainly handles editor camera movement
     */
    void handleMouseInput();

    Registry *registry;
    ResourceManager *factory;

    float mCameraSpeed{1.f};
    float mMoveSpeed{10.f};
    float mCameraRotateSpeed{30.f};
    bool firstRMB{true};
    // last position of the mouse in editor, for calculating deltas to move the camera with
    QPoint lastPos;
    bool mEnteredWindow{false};
    bool mIsConfined{false};
    bool buildableDebug{false};

    GLuint draggedEntity{0};
    GLuint lastHitEntity{0};
    vec3 red{2.0f, 0, 0}, green{0, 2.0f, 0};
    vec3 origColor{0.57f, 0.57f, 0.57f};
    bool mIsDragging{false};

    cjk::Ref<CameraController> mEditorCamController;
    std::vector<cjk::Ref<GameCameraController>> mGameCameraControllers;
    bool mActiveGameCamera{false};

    RenderWindow *mRenderWindow;

    Input mPlayerController;
    vec3 mDesiredVelocity;
    Input editorInput;
    /**
     * @brief handlePlayerController mainly deals with WASD and other player control type events
     * @param dt
     */
    void handlePlayerController(DeltaTime dt);
    void setCameraSpeed(float value);
    /**
     * @brief inputKeyPress takes an Input variable based on whether the editor is in play mode or not
     * @param event
     * @param input
     */
    void inputKeyPress(QKeyEvent *event, Input &input);
    /**
     * @brief inputKeyRelease takes an Input variable based on whether the editor is in play mode or not
     * @param event
     * @param input
     */
    void inputKeyRelease(QKeyEvent *event, Input &input);

    void inputMousePress(QMouseEvent *event, Input &input);
    void inputMouseRelease(QMouseEvent *event, Input &input);
    void snapToObject();
    void confineMouseToScreen(DeltaTime dt);
    /**
     * @brief dragEntity - move entity by mouse raycast when activated
     * If the mouse raycast hits an AABB or sphere collider it will place the entity on top of that collider, as close to the mouse as possible
     * @param entity
     */
    void dragEntity(GLuint entity);
    void updateBuildable(GLuint entityID);
    void spawnTower();
    void placeTower();
};

#endif // INPUTSYSTEM_H
