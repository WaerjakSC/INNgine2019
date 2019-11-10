#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H
#include "core.h"
#include "isystem.h"
#include "pool.h"
using namespace cjk;
class RenderWindow;
class Registry;
class ResourceManager;
class MovementSystem;
class Raycast;
class CameraController;
class GameCameraController;
class InputSystem : public QObject, public ISystem {
    Q_OBJECT
public:
    InputSystem(RenderWindow *window);
    void update(DeltaTime dt = 0.016) override;
    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    Input playerController() const;
    Input &playerController();

    Ref<CameraController> editorCamController() const;
    void setEditorCamController(const Ref<CameraController> &editorCamController);

    Ref<GameCameraController> gameCameraController() const;
    void setGameCameraController(const Ref<GameCameraController> &gameCameraController, GLuint gameCamMeshID);

    void onResize(float aspectRatio);
    GLuint player() const;

    void setPlayer(const GLuint &player);

signals:
    void snapSignal();
    void rayHitEntity(GLuint entityID);
    void closeEngine();

private:
    /**
     * @brief handleKeyInput takes care of certain hard-coded key inputs
     */
    void handleKeyInput();
    /**
     * @brief handleMouseInput mainly handles editor camera movement
     */
    void handleMouseInput();

    float mCameraSpeed{1.f};
    float mMoveSpeed{10.f};
    float mCameraRotateSpeed{30.f};
    bool firstRMB{true};
    QPoint lastPos; // last position of the mouse in editor, for calculating deltas to move the camera with
    Raycast *ray;

    Registry *registry{};
    ResourceManager *factory;
    Ref<CameraController> mEditorCamController;
    Ref<GameCameraController> mGameCameraController;

    RenderWindow *mRenderWindow;

    GLuint mPlayer{0};
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
};

#endif // INPUTSYSTEM_H
