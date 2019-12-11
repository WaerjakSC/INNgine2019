#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H
#include "components.h"
#include "core.h"
#include "isystem.h"
class RenderWindow;
class Registry;
class ResourceManager;
class CameraController;
class GameCameraController;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
/**
 * @brief The InputSystem class is in charge of input events and the various camera controllers.
 */
class InputSystem : public QObject, public ISystem {
    Q_OBJECT
    using vec3 = gsl::Vector3D;

public:
    InputSystem(RenderWindow *window, cjk::Ref<CameraController> editorController);
    void update(DeltaTime dt = 0.016) override;
    void updatePlayOnly(DeltaTime deltaTime = 0.016);
    /**
     * @brief init creates a GameCameraController for each entity containing a GameCamera component.
     * @param aspectRatio
     */
    void init(float aspectRatio = 1.92f);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    /**
     * @brief gameCameraControllers getter for the vector of game camera controllers
     * @return
     */
    std::vector<cjk::Ref<GameCameraController>> gameCameraControllers() const;
    /**
     * @brief currentCameraController finds and returns the active game camera controller if in play mode, otherwise returns the editor camera controller
     * @return
     */
    cjk::Ref<CameraController> currentCameraController();

    /**
     * @brief onResize called when the RenderWindow is resized. Updates the aspectRatio of all camera controllers.
     * @param aspectRatio
     */
    void onResize(float aspectRatio);
    /**
     * @brief reset Resets editor and play variables, for instance when switching from Editor to Play or vice versa.
     */
    void reset();
    /**
     * @brief setBuildableDebug If true, turns on colored tiles for debugging the buildable mode
     * @param value
     */
    void setBuildableDebug(bool value);

    cjk::Ref<CameraController> editorCamController() const;

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
    bool mBuildableDebug{false};

    GLuint draggedEntity{0};
    GLuint lastHitEntity{0};
    vec3 red{2.0f, 0, 0}, green{0, 2.0f, 0};
    vec3 origColor{0.57f, 0.57f, 0.57f};
    bool mIsDragging{false};

    RenderWindow *mRenderWindow;
    cjk::Ref<CameraController> mEditorCamController;
    std::vector<cjk::Ref<GameCameraController>> mGameCameraControllers;
    bool mActiveGameCamera{false};

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
