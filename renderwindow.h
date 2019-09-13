#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include "camera.h"

#include "Components/inputcomponent.h"
#include "gameobject.h"
#include "resourcemanager.h"
#include "texture.h"
#include <QElapsedTimer>
#include <QTimer>
#include <QWindow>
#include <chrono>

class QOpenGLContext;
class Shader;
class MainWindow;
class LightObject;
class RenderSystem;
class MovementSystem;

/// This inherits from QWindow to get access to the Qt functionality and
/// OpenGL surface.
/// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
/// This is the same as using glad and glw from general OpenGL tutorials
class RenderWindow : public QWindow, protected QOpenGLFunctions_4_1_Core {
    Q_OBJECT
public:
    RenderWindow();
    RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow);
    ~RenderWindow() override;

    QOpenGLContext *context() { return mContext; }

    void exposeEvent(QExposeEvent *) override;
    void toggleWireframe();

    void checkForGLerrors();
    void setCameraSpeed(float value);

    ResourceManager &factory() const;

private slots:
    void render();
    void updateScene();

private:
    void init();

    QOpenGLContext *mContext{nullptr};
    bool mInitialized{false};

    std::vector<GameObject *> mGameObjects;
    RenderSystem *mRenderer;
    MovementSystem *mMoveSys;
    ResourceManager &mFactory;
    GameObject *mPlayer; //the controllable object

    GLuint mLight;

    InputComponent *mInput;

    Camera *mCurrentCamera{nullptr};

    bool mWireframe{false};

    //Input mInput;

    float mCameraSpeed{0.01f};
    float mCameraRotateSpeed{0.1f};
    int mMouseXlast{0};
    int mMouseYlast{0};

    QTimer *mRenderTimer{nullptr}; //timer that drives the gameloop
    QElapsedTimer mTimeStart;      //time variable that reads the actual FPS

    float mAspectratio{1.f};

    MainWindow *mMainWindow{nullptr}; //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    void calculateFramerate();

    void startOpenGLDebugger();

    void handleInput();

    std::chrono::high_resolution_clock::time_point mLastTime;

protected:
    //The QWindow that we inherit from has these functions to capture
    // mouse and keyboard. Uncomment to use
    //
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // RENDERWINDOW_H
