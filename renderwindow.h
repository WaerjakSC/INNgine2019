#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include "camera.h"
#include "cameracontroller.h"
#include "core.h"
#include "texture.h"
#include <QElapsedTimer>
#include <QTimer>
#include <QWindow>
#include <chrono>
#include <memory>
using namespace cjk;
class QOpenGLContext;
class Shader;
class MainWindow;
class RenderSystem;
class MovementSystem;
class LightSystem;
class SoundSystem;
class InputSystem;
class CollisionSystem;
class ParticleSystem;
class AISystem;
class ResourceManager;
class ScriptSystem;
class Registry;
class Raycast;
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

    void checkForGLerrors();
    void setCameraSpeed(float value);
    void playSound();

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    void toggleWireframe();
    void toggleXYZ();

private slots:
    void render();

private:
    void init();

    QOpenGLContext *mContext{nullptr};
    bool mInitialized{false};

    Ref<RenderSystem> mRenderer;
    Ref<ParticleSystem> mParticleSystem;
    Ref<MovementSystem> mMoveSystem;
    Ref<LightSystem> mLightSystem;
    Ref<SoundSystem> mSoundSystem;
    Ref<InputSystem> mInputSystem;
    Ref<CollisionSystem> mCollisionSystem;
    Ref<ScriptSystem> mScriptSystem;
    Ref<AISystem> mAIsystem;

    ResourceManager *mFactory;
    Registry *mRegistry;

    Ref<CameraController> mEditorCameraController{nullptr};

    GLuint xyz; // Refers to the colored lines pointing in the X, Y and Z directions.
    bool mWireframe{false};

    //Input mInput;

    QTimer *mRenderTimer{nullptr}; //timer that drives the gameloop
    QElapsedTimer mTimeStart;      //time variable that reads the actual FPS
    QElapsedTimer mTime;
    float mLastFrameTime{0};

    float mAspectratio{1.f};

    MainWindow *mMainWindow{nullptr}; //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    void calculateFramerate();

    void startOpenGLDebugger();

    std::chrono::high_resolution_clock::time_point mLastTime;
    friend class MainWindow;

    void Cull(const Camera::Frustum &f);
    Camera mCamera;

    friend class MainWindow;

protected:
    //The QWindow that we inherit from has these functions to capture
    // mouse and keyboard. Uncomment to use
    //
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;
};

#endif // RENDERWINDOW_H
