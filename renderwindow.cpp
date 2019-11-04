#include "renderwindow.h"
#include "innpch.h"
#include "mainwindow.h"
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QStatusBar>
#include <QTimer>
#include <chrono>
#include <iostream>
#include <thread> //for sleep_for

#include "deltaTime.h"
#include "inputsystem.h"
#include "lightsystem.h"
#include "movementsystem.h"
#include "rendersystem.h"
#include "soundsystem.h"

#include "colorshader.h"
#include "phongshader.h"
#include "textureshader.h"

#include "raycast.h"
#include "registry.h"
#include "resourcemanager.h"
#include "scene.h"

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false),
      mFactory(ResourceManager::instance()), mRegistry(Registry::instance()),
      mMainWindow(mainWindow) {
    //This is sent to QWindow:
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    //Make the OpenGL context
    mContext = new QOpenGLContext(this);
    //Give the context the wanted OpenGL format (v4.1 Core)
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }
    //Make the gameloop timer:
    mRenderTimer = new QTimer(this);
}

RenderWindow::~RenderWindow() {
    mSoundSystem->cleanUp();
}

/// Sets up the general OpenGL stuff and the buffers needed to render a triangle
void RenderWindow::init() {
    //Connect the gameloop timer to the render function:
    connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(render()));

    //********************** General OpenGL stuff **********************

    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!
    if (!mContext->makeCurrent(this)) {
        qDebug() << "makeCurrent() failed";
        return;
    }

    //just to make sure we don't init several times
    //used in exposeEvent()
    if (!mInitialized)
        mInitialized = true;

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Print render version info:
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

    //Start the Qt OpenGL debugger
    //Really helpfull when doing OpenGL
    //Supported on most Windows machines
    //reverts to plain glGetError() on Mac and other unsupported PCs
    // - can be deleted
    startOpenGLDebugger();

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);              //enables depth sorting - must use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_CULL_FACE);               //draws only front side of models - usually what you want -
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f); //color used in glClear GL_COLOR_BUFFER_BIT

    mFactory->setMainWindow(mMainWindow);

    //********************** Set up camera **********************
    float aspectRatio = static_cast<float>(width()) / height();
    mEditorCameraController = std::make_shared<CameraController>(aspectRatio);
    mEditorCameraController->setPosition(vec3(0.f, 8.f, 15.0f));
    mFactory->setCurrentCameraController(mEditorCameraController);

    //Compile shaders - init them with reference to current camera:
    mFactory->loadShader<ColorShader>(mEditorCameraController);
    mFactory->loadShader<TextureShader>(mEditorCameraController);
    mFactory->loadShader<PhongShader>(mEditorCameraController);
    //**********************  Texture stuff: **********************

    mFactory->loadTexture("white.bmp");
    mFactory->loadTexture("gnome.bmp");
    mFactory->loadTexture("skybox.bmp");

    //Set the textures loaded to a texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mFactory->getTexture("white.bmp")->id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mFactory->getTexture("gnome.bmp")->id());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mFactory->getTexture("skybox.bmp")->id());

    // Set up the systems.
    mRenderer = mRegistry->registerSystem<RenderSystem>(mFactory->getShaders());
    mMoveSystem = mRegistry->registerSystem<MovementSystem>();
    mLightSystem = mRegistry->registerSystem<LightSystem>(mFactory->getShader<PhongShader>());
    mInputSystem = mRegistry->registerSystem<InputSystem>(this);
    mInputSystem->setEditorCamController(mEditorCameraController);
    mSoundSystem = mRegistry->registerSystem<SoundSystem>();
    mSoundSystem->createContext();
    //********************** Making the objects to be drawn **********************
    xyz = mFactory->makeXYZ();
    mFactory->loadLastProject();
    mFactory->makeLASMap();

    mMainWindow->setWindowTitle("Project: " + mFactory->getProjectName() + " - Current Scene: " + mFactory->getCurrentScene());
    mSoundSystem->init();
    mMoveSystem->init();

    mLightSystem->init(mRegistry->getEntity(mFactory->getSceneLoader()->mLight));
    mRenderer->init();

    connect(mRegistry->getSystem<InputSystem>().get(), &InputSystem::snapSignal, mMainWindow, &MainWindow::snapToObject);
    connect(mRegistry->getSystem<InputSystem>().get(), &InputSystem::rayHitEntity, mMainWindow, &MainWindow::mouseRayHit);
    connect(mRegistry->getSystem<InputSystem>().get(), &InputSystem::closeEngine, mMainWindow, &MainWindow::closeEngine);
}

///Called each frame - doing the rendering
void RenderWindow::render() {

    float time = static_cast<float>(mTime.elapsed()) / 1000.f;
    DeltaTime dt = time - mLastFrameTime;
    mLastFrameTime = time;

    mInputSystem->update(dt);

    mTimeStart.restart();        //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    //to clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!ResourceManager::instance()->isLoading()) { // Not sure if this is necessary, but we wouldn't want to try rendering something before the scene is done loading everything
        mSoundSystem->update(dt);
        if (mFactory->isPlaying()) {
            mMoveSystem->update(dt);
        }
        mLightSystem->update(dt);
        mRenderer->update(dt);
    }
    //Calculate framerate before
    // checkForGLerrors() because that takes a long time
    // and before swapBuffers(), else it will show the vsync time
    calculateFramerate();

    //using our expanded OpenGL debugger to check if everything is OK.
    //    checkForGLerrors();

    //Qt require us to call this swapBuffers() -function.
    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.
    //    auto start = std::chrono::high_resolution_clock::now();
    mContext->swapBuffers(this);
}
void RenderWindow::snapToObject(int eID) {
    mEditorCameraController->goTo(mMoveSystem->getAbsolutePosition(eID));
}

Ref<RenderSystem> RenderWindow::renderer() const {
    return mRenderer;
}
Ref<MovementSystem> RenderWindow::movement() const {
    return mMoveSystem;
}
//This function is called from Qt when window is exposed (shown)
//and when it is resized
//exposeEvent is a overridden function from QWindow that we inherit from
void RenderWindow::exposeEvent(QExposeEvent *) {
    if (!mInitialized)
        init();

    //If the window actually is exposed to the screen we start the main loop
    //isExposed() is a function in QWindow
    if (isExposed()) {
        //This timer runs the actual MainLoop
        //16 means 16ms = 60 Frames pr second (should be 16.6666666 to be exact..)
        mRenderTimer->start(1);
        mTimeStart.start();
        mTime.start();
    }
    //This is just to support modern screens with "double" pixels
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));
    float aspectRatio = static_cast<float>(width()) / height();
    mInputSystem->onResize(aspectRatio);
}

//Simple way to turn on/off wireframe mode
//Not totally accurate, but draws the objects with
//lines instead of filled polygons
void RenderWindow::toggleWireframe() {
    mWireframe = !mWireframe;
    if (mWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //turn on wireframe mode
        glDisable(GL_CULL_FACE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //turn off wireframe mode
        glEnable(GL_CULL_FACE);
    }
}
// Whether something should be rendered or not is handled in RenderSystem
void RenderWindow::toggleXYZ() {
    mRenderer->toggleRendered(xyz);
}

//The way this is set up is that we start the clock before doing the draw call,
//and check the time right after it is finished (done in the render function)
//This will approximate what framerate we COULD have.
//The actual frame rate on your monitor is limited by the vsync and is probably 60Hz
void RenderWindow::calculateFramerate() {
    long long nsecElapsed = mTimeStart.nsecsElapsed();
    static int frameCount{0}; //counting actual frames for a quick "timer" for the statusbar

    if (mMainWindow) //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 30) //once pr 30 frames = update the message twice pr second (on a 60Hz monitor)
        {
            if (!mMainWindow->mShowingMsg) {
                //showing some statistics in status bar
                mMainWindow->statusBar()->showMessage(" Time pr FrameDraw: " +
                                                      QString::number(nsecElapsed / 1000000., 'g', 4) + " ms  |  " +
                                                      "FPS (approximated): " + QString::number(1E9 / nsecElapsed, 'g', 7));
            }
            frameCount = 0; //reset to show a new message in 60 frames
        }
    }
}

/// Uses QOpenGLDebugLogger if this is present
/// Reverts to glGetError() if not
void RenderWindow::checkForGLerrors() {
    if (mOpenGLDebugLogger) {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
            qDebug() << message;
    } else {
        GLenum err = GL_NO_ERROR;
        while ((err = glGetError()) != GL_NO_ERROR) {
            qDebug() << "glGetError returns " << err;
        }
    }
}

/// Tries to start the extended OpenGL debugger that comes with Qt
void RenderWindow::startOpenGLDebugger() {
    QOpenGLContext *temp = this->context();
    if (temp) {
        QSurfaceFormat format = temp->format();
        if (!format.testOption(QSurfaceFormat::DebugContext))
            qDebug() << "This system can not use QOpenGLDebugLogger, so we revert to glGetError()";

        if (temp->hasExtension(QByteArrayLiteral("GL_KHR_debug"))) {
            qDebug() << "System can log OpenGL errors!";
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                qDebug() << "Started OpenGL debug logger!";
        }

        if (mOpenGLDebugLogger)
            mOpenGLDebugLogger->disableMessages(QOpenGLDebugMessage::APISource, QOpenGLDebugMessage::OtherType, QOpenGLDebugMessage::NotificationSeverity);
    }
}

/**
 * @brief RenderWindow::Cull, Cull furries right now
 * @param f, the frustum
 */
void RenderWindow::Cull(const Camera::Frustum &f) {
    auto view = mRegistry->view<Mesh, Collision>();
    for (auto entity : view) {
        auto &collider = view.get<Collision>(entity);
        const AABB &bounds = dynamic_cast<AABB &>(collider);
        if (mInputSystem->editorCamController()->getCamera().getFrustum().Intersects(f, bounds)) {
            view.get<Mesh>(entity).mRendered = true;
        } else
            view.get<Mesh>(entity).mRendered = false;
    }
}

void RenderWindow::keyPressEvent(QKeyEvent *event) {
    if (mInputSystem)
        mInputSystem->keyPressEvent(event);
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event) {
    if (mInputSystem)
        mInputSystem->keyReleaseEvent(event);
}

void RenderWindow::mousePressEvent(QMouseEvent *event) {
    if (mInputSystem)
        mInputSystem->mousePressEvent(event);
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (mInputSystem)
        mInputSystem->mouseReleaseEvent(event);
}
void RenderWindow::mouseMoveEvent(QMouseEvent *event) {
    if (mInputSystem)
        mInputSystem->mouseMoveEvent(event);
}
void RenderWindow::wheelEvent(QWheelEvent *event) {
    if (mInputSystem)
        mInputSystem->wheelEvent(event);
}
