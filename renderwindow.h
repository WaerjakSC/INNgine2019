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

class QOpenGLContext;
class Shader;
class MainWindow;
class RenderSystem;
class MovementSystem;
class SoundSource;
class SoundManager;
class LightSystem;
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
    void toggleWireframe();

    void checkForGLerrors();
    void setCameraSpeed(float value);
    void playSound();

    RenderSystem *renderer() const;

    MovementSystem *movement() const;

    SoundSource *mStereoSound{};

    SoundManager *soundManager() const;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    void snapToObject(int eID);
    void save();
    void load();
    void play();
    void pause();
    void stop();
    void saveAs();
    void saveProject();
    void loadProject();
private slots:
    void render();
    void changeMsg();
signals:
    void snapSignal();
    void rayHitEntity(GLuint eID);

private:
    void init();

    QOpenGLContext *mContext{nullptr};
    bool mInitialized{false};

    std::unique_ptr<RenderSystem> mRenderer;
    std::unique_ptr<MovementSystem> mMoveSys;
    std::shared_ptr<LightSystem> mLightSys;

    ResourceManager *mFactory;
    Registry *mRegistry;
    SoundManager *mSoundManager;
    Raycast *ray;

    Entity *mPlayer; //the controllable object

    GLuint mLight;

    Input *mInput;

    Camera *mCurrentCamera{nullptr};

    bool mWireframe{false};
    bool mShowingMsg{false};
    bool mIsPlaying{false};
    bool mPaused{false}; // Don't make a snapshot if it was just restarted from a pause

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
    friend class MainWindow;

    std::vector<GLuint *> Cull(const Camera::Frustum &f);

    void showMessage(const QString &message);

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
