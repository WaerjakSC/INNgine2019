﻿#include "renderwindow.h"
#include "innpch.h"
#include "inputsystem.h"
#include "scene.h"
#include "soundmanager.h"
#include "soundsource.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QStatusBar>
#include <QTimer>
#include <QToolButton>
#include <chrono>
#include <iostream>
#include <thread> //for sleep_for

#include "mainwindow.h"

#include "colorshader.h"
#include "lightsystem.h"
#include "movementsystem.h"
#include "phongshader.h"
#include "raycast.h"
#include "registry.h"
#include "rendersystem.h"
#include "soundsystem.h"
#include "textureshader.h"
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
    SoundManager::instance()->cleanUp();
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
    mFactory->setCurrentCamera(new Camera());
    mCurrentCamera = mFactory->getCurrentCamera();
    mCurrentCamera->setPosition(vec3(0.f, 8.f, 15.0f));
    //    mCurrentCamera->yaw(45.f);
    mCurrentCamera->pitch(25.f);

    //Compile shaders:
    mFactory->loadShader(Color);
    mFactory->loadShader(Tex);
    mFactory->loadShader(Phong);
    //new system - shader sends uniforms so needs to get the view and projection matrixes from camera
    mFactory->getShader(ShaderType::Color)->setCurrentCamera(mCurrentCamera);
    mFactory->getShader(ShaderType::Tex)->setCurrentCamera(mCurrentCamera);
    mFactory->getShader(ShaderType::Phong)->setCurrentCamera(mCurrentCamera);
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
    mMoveSys = mRegistry->registerSystem<MovementSystem>();
    mLightSys = mRegistry->registerSystem<LightSystem>(static_cast<PhongShader *>(mFactory->getShader(ShaderType::Phong)));
    mInput = mRegistry->registerSystem<InputSystem>(this);
    mSoundSys = mRegistry->registerSystem<SoundSystem>();

    //********************** Making the objects to be drawn **********************
    xyz = mFactory->makeXYZ();
    mFactory->loadLastProject();

    //    GLuint cb = mFactory->make3DObject("cube.obj", ShaderType::Phong); // WHY DOES THIS CAUSE PHONG SHADING TO WORK?

    mMainWindow->setWindowTitle(mFactory->getProjectName() + " - Current Scene: " + mFactory->getCurrentScene());

    mMoveSys->init();
    mSoundSys->init();
    // These components don't have a scene thingy yet
    mRegistry->addComponent<Input>(mFactory->getSceneLoader()->controllerID);
    mRegistry->addComponent<Sound>(2, "gnomed.wav", true, 1.0f);
    mSoundSys->prepareSounds();
    mRenderer->init();
    //    if (mRegistry->getEntity(cb))    // Super scuffed workaround until I figure out why manually creating a 3d phong object "turns on" phong shading
    //        mRegistry->removeEntity(cb); // Removing the created object here lets me keep the shading
    mLightSys->init();

    mInput->setPlayerController(mFactory->getSceneLoader()->controllerID);

    connect(mRegistry->getSystem<InputSystem>(), &InputSystem::snapSignal, mMainWindow, &MainWindow::snapToObject);
    connect(mRegistry->getSystem<InputSystem>(), &InputSystem::rayHitEntity, mMainWindow, &MainWindow::mouseRayHit);
    connect(mRegistry->getSystem<InputSystem>(), &InputSystem::closeEngine, mMainWindow, &MainWindow::closeEngine);
}

///Called each frame - doing the rendering
void RenderWindow::render() {

    mCurrentCamera->update();

    mTimeStart.restart();        //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    //to clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!ResourceManager::instance()->isLoading()) { // Not sure if this is necessary, but we wouldn't want to try rendering something before the scene is done loading everything
        if (mFactory->isPlaying()) {
            mSoundSys->update();
            mMoveSys->update();
        }
        mRenderer->update();
        mLightSys->update();
        mInput->update();
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
    mCurrentCamera->goTo(mMoveSys->getAbsolutePosition(eID));
}

RenderSystem *RenderWindow::renderer() const {
    return mRenderer;
}
MovementSystem *RenderWindow::movement() const {
    return mMoveSys;
}
//This function is called from Qt when window is exposed (shown)
//and when it is resized
//exposeEvent is a overridden function from QWindow that we inherit from
void RenderWindow::exposeEvent(QExposeEvent *) {
    if (!mInitialized)
        init();

    //This is just to support modern screens with "double" pixels
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));

    //If the window actually is exposed to the screen we start the main loop
    //isExposed() is a function in QWindow
    if (isExposed()) {
        //This timer runs the actual MainLoop
        //16 means 16ms = 60 Frames pr second (should be 16.6666666 to be exact..)
        mRenderTimer->start(1);
        mTimeStart.start();
    }
    mAspectratio = static_cast<float>(width()) / height();
    //    qDebug() << mAspectratio;
    mCurrentCamera->mProjectionMatrix.perspective(45.f, mAspectratio, 1.f, 100.f);
    //    qDebug() << mCamera.mProjectionMatrix;
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
//std::vector<GLuint> RenderWindow::Cull(const Camera::Frustum &f) {
//    std::vector<GLuint> result;

//    for(auto entity : Registry::instance()->getEntities())
//    GLuint entityID = entity.first;

//    for (int i = 0; i < objects.size(); i++) {
//        oBB bounds = GetOBB(*(objects[i]));
//        if (Intersects(f, bounds)) {
//            result.push_back(objects[i]);
//        }
//    }
//    return result;
//}
void RenderWindow::keyPressEvent(QKeyEvent *event) {
    if (mInput)
        mInput->keyPressEvent(event);
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event) {
    if (mInput)
        mInput->keyReleaseEvent(event);
}

void RenderWindow::mousePressEvent(QMouseEvent *event) {
    if (mInput)
        mInput->mousePressEvent(event);
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (mInput)
        mInput->mouseReleaseEvent(event);
}
void RenderWindow::mouseMoveEvent(QMouseEvent *event) {
    if (mInput)
        mInput->mouseMoveEvent(event);
}
void RenderWindow::wheelEvent(QWheelEvent *event) {
    if (mInput)
        mInput->wheelEvent(event);
}
