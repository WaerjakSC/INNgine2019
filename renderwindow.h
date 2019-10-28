#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include "camera.h"
#include "entity.h"
#include "resourcemanager.h"
#include "texture.h"
#include <QElapsedTimer>
#include <QTimer>
#include <QWindow>
#include <chrono>
#include <memory>
typedef gsl::Vector3D vec3;
class QOpenGLContext;
class Shader;
class MainWindow;
class RenderSystem;
class MovementSystem;
class SoundSource;
class SoundManager;
class LightSystem;
class InputSystem;
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

    RenderSystem *renderer() const;

    MovementSystem *movement() const;

    SoundSource *mStereoSound{};

    SoundManager *soundManager() const;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    bool isPlaying() const;

public slots:
    void snapToObject(int eID);

    void toggleWireframe();
    void toggleXYZ();

private slots:
    void render();
    void changeMsg();

private:
    void init();

    QOpenGLContext *mContext{nullptr};
    bool mInitialized{false};

    RenderSystem *mRenderer;
    MovementSystem *mMoveSys;
    LightSystem *mLightSys;

    ResourceManager *mFactory;
    Registry *mRegistry;
    SoundManager *mSoundManager;
    Raycast *ray;

    Entity *mPlayer; //the controllable object

    GLuint mLight;

    InputSystem *mInput;

    Camera *mCurrentCamera{nullptr};

    GLuint xyz; // Refers to the colored lines pointing in the X, Y and Z directions.
    bool mWireframe{false};

    //Input mInput;

    float mCameraSpeed{0.01f};
    float mCameraRotateSpeed{30.f};
    bool firstRMB{true};
    QPoint lastPos;

    QTimer *mRenderTimer{nullptr}; //timer that drives the gameloop
    QElapsedTimer mTimeStart;      //time variable that reads the actual FPS

    float mAspectratio{1.f};

    MainWindow *mMainWindow{nullptr}; //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    void calculateFramerate();

    void startOpenGLDebugger();

    std::chrono::high_resolution_clock::time_point mLastTime;
    friend class MainWindow;

    std::vector<GLuint *> Cull(const Camera::Frustum &f);

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
