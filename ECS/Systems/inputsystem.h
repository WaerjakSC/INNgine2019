#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H
#include "isystem.h"
#include "pool.h"
class RenderWindow;
class Registry;
class ResourceManager;
class MovementSystem;
class Raycast;
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

signals:
    void snapSignal();
    void rayHitEntity(GLuint entityID);
    void closeEngine();
private slots:
    //    void changeMsg();

private:
    void handleKeyInput();
    void handleMouseInput();

    float mCameraSpeed{0.01f};
    float mCameraRotateSpeed{30.f};
    bool firstRMB{true};
    QPoint lastPos;
    Raycast *ray;

    Registry *registry{};
    ResourceManager *factory;
    MovementSystem *mMoveSys{nullptr};
    GLuint mPlayerController;
    Camera *mCurrentCamera{nullptr};
    RenderWindow *mRenderWindow;
    Input editorInput;
    void handlePlayerController(float deltaTime);
    void setCameraSpeed(float value);
    void inputKeyRelease(QKeyEvent *event, Input &input);
    void inputKeyPress(QKeyEvent *event, Input &input);
    void inputMousePress(QMouseEvent *event, Input &input);
    void inputMouseRelease(QMouseEvent *event, Input &input);
};

#endif // INPUTSYSTEM_H
