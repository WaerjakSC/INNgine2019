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
    void update(float deltaTime = 0.016) override;
    void keyPressEvent(QKeyEvent *event);

    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void setPlayerController(const GLuint &playerController);

    GLuint playerController() const;

    Ref<CameraController> editorCamController() const;
    void setEditorCamController(const Ref<CameraController> &editorCamController);

    Ref<GameCameraController> gameCameraController() const;
    void setGameCameraController(const Ref<GameCameraController> &gameCameraController);

    void onResize(float aspectRatio);
signals:
    void snapSignal();
    void rayHitEntity(GLuint entityID);
    void closeEngine();
private slots:
    //    void changeMsg();

private:
    void handleKeyInput();
    void handleMouseInput();

    float mCameraSpeed{1.f};
    float mMoveSpeed{10.f};
    float mCameraRotateSpeed{30.f};
    bool firstRMB{true};
    QPoint lastPos;
    Raycast *ray;

    Registry *registry{};
    ResourceManager *factory;
    Ref<CameraController> mEditorCamController;
    Ref<GameCameraController> mGameCameraController;
    RenderWindow *mRenderWindow;

    GLuint mPlayerController;
    Input editorInput;

    void handlePlayerController(float deltaTime);
    void setCameraSpeed(float value);
    void inputKeyRelease(QKeyEvent *event, Input &input);
    void inputKeyPress(QKeyEvent *event, Input &input);
    void inputMousePress(QMouseEvent *event, Input &input);
    void inputMouseRelease(QMouseEvent *event, Input &input);
};

#endif // INPUTSYSTEM_H
