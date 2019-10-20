#include "renderwindow.h"
#include "innpch.h"
#include "scene.h"
#include "soundmanager.h"
#include "soundsource.h"
#include "view.h"

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
#include "renderview.h"
#include "textureshader.h"

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false),
      mFactory(ResourceManager::instance()), mRegistry(Registry::instance()), mSoundManager(SoundManager::instance()),
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
    // Create Resource Manager instance
    mInput = new Input();
    mInput->setMainWindow(mMainWindow);
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
    //makes the soundmanager
    //it is a Singleton!
    mSoundManager->init();
    //Compile shaders:
    mFactory->loadShader(Color);
    mFactory->loadShader(Tex);
    mFactory->loadShader(Phong);
    //********************** Set up camera **********************
    mCurrentCamera = new Camera();
    mCurrentCamera->setPosition(vec3(1.f, 1.f, 4.4f));
    //    mCurrentCamera->yaw(45.f);
    //    mCurrentCamera->pitch(5.f);
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
    mFactory->setLightSystem(mLightSys);
    ray = new Raycast(this, mCurrentCamera);

    //********************** Making the objects to be drawn **********************
    mFactory->loadLastProject(); // loadScene should be placed somewhere in init() rather than saveScene

    mMainWindow->setWindowTitle(mFactory->getProjectName() + " - Current Scene: " + mFactory->getCurrentScene());
    mLight = mFactory->getSceneLoader()->controllerID;
    mRenderer->init();
    mMoveSys->init();
    View<Transform> test(mMoveSys->mTransforms);
    auto transform = test.get<Transform>(4);
    qDebug() << transform.parentID;

    mStereoSound = mSoundManager->createSource(
        "Explosion", Vector3(0.0f, 0.0f, 0.0f),
        "../INNgine2019/Assets/Sounds/gnomed.wav", true, 1.0f);
}

bool RenderWindow::isPlaying() const {
    return mIsPlaying;
}

///Called each frame - doing the rendering
void RenderWindow::render() {
    //input
    handleInput();

    mCurrentCamera->update();

    mTimeStart.restart();        //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    //to clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!ResourceManager::instance()->isLoading()) { // Not sure if this is necessary, but we wouldn't want to try rendering something before the scene is done loading everything
        if (mIsPlaying) {
            mMoveSys->update();
        }
        mRenderer->update();
        mLightSys->update();
        mSoundManager->updateListener(mCurrentCamera->position(), vec3(1), mCurrentCamera->forward(), mCurrentCamera->up());
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
    return mRenderer.get();
}
MovementSystem *RenderWindow::movement() const {
    return mMoveSys.get();
}

SoundManager *RenderWindow::soundManager() const {
    return mSoundManager;
}
void RenderWindow::playSound() {
    //plays the sounds
    mStereoSound->play();
    mStereoSound->setPosition(Vector3(4.f, 2.f, -3.5f));
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
            if (!mShowingMsg) {
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

void RenderWindow::keyPressEvent(QKeyEvent *event) {
    mInput->keyPressEvent(event);
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event) {
    mInput->keyReleaseEvent(event);
}
void RenderWindow::showMessage(const QString &message) {
    mMainWindow->statusBar()->showMessage(message, 1000);
    mShowingMsg = true;
    QTimer::singleShot(1000, this, &RenderWindow::changeMsg);
}

void RenderWindow::save() {
    mFactory->getSceneLoader()->saveScene(mFactory->getCurrentScene());
    showMessage("Saved Scene!");
}
void RenderWindow::saveAs() {
    QFileInfo file(QFileDialog::getSaveFileName(mMainWindow, tr("Save Scene"), QString::fromStdString(gsl::sceneFilePath), tr("JSON files (*.json)")));
    if (file.fileName().isEmpty())
        return;
    mFactory->getSceneLoader()->saveScene(file.fileName());
    mMainWindow->setWindowTitle(mFactory->getProjectName() + " - Current Scene: " + mFactory->getCurrentScene());
    showMessage("Saved Scene!");
}
void RenderWindow::load() {
    QFileInfo file(QFileDialog::getOpenFileName(mMainWindow, tr("Load Scene"), QString::fromStdString(gsl::sceneFilePath), tr("JSON files (*.json)")));
    if (file.fileName().isEmpty())
        return;
    stop(); // Stop the editor if it's in play
    mMainWindow->clearEditor();
    mFactory->getSceneLoader()->loadScene(file.fileName());
    mFactory->setCurrentScene(file.fileName());
    mMoveSys->init();
    mMainWindow->setWindowTitle(mFactory->getProjectName() + " - Current Scene: " + mFactory->getCurrentScene());
    showMessage("Loaded Scene!");
}
void RenderWindow::loadProject() {
    QFileInfo file(QFileDialog::getOpenFileName(mMainWindow, tr("Load Project"), QString::fromStdString(gsl::settingsFilePath), tr("JSON files (*.json)")));
    if (file.fileName().isEmpty())
        return;
    stop(); // Stop the editor if it's in play
    mMainWindow->clearEditor();
    mFactory->loadProject(file.fileName());
    mMainWindow->setWindowTitle(mFactory->getProjectName() + " - Current Scene: " + mFactory->getCurrentScene());
    mMoveSys->init();
    showMessage("Loaded Project: " + mFactory->getProjectName());
}
void RenderWindow::saveProject() {
    mFactory->saveProjectSettings(QString::fromStdString(gsl::settingsFilePath + mFactory->getProjectName().toStdString()));
    showMessage("Saved Project!");
}
void RenderWindow::play() {
    if (!mIsPlaying) {
        if (mPaused) { // Only make snapshot if not resuming from pause
            mPaused = false;
        } else
            mRegistry->makeSnapshot();
        mIsPlaying = true;
        mMainWindow->play->setEnabled(false);
        mMainWindow->pause->setEnabled(true);
        mMainWindow->stop->setEnabled(true);
        playSound();
    }
}
void RenderWindow::pause() {
    if (mIsPlaying) {
        mIsPlaying = false;
        mPaused = true;
        mMainWindow->play->setEnabled(true);
        mMainWindow->pause->setEnabled(false);
        mStereoSound->pause();
    }
}
void RenderWindow::stop() {
    if (mIsPlaying) {
        mRegistry->loadSnapshot();
        mMoveSys->init();
        mIsPlaying = false;
        mMainWindow->insertEntities();
        mMainWindow->play->setEnabled(true);
        mMainWindow->pause->setEnabled(false);
        mMainWindow->stop->setEnabled(false);
        mStereoSound->stop();
    }
}
void RenderWindow::setCameraSpeed(float value) {
    mCameraSpeed += value;
    //Keep within min and max values
    if (mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.3f)
        mCameraSpeed = 0.3f;
}
void RenderWindow::changeMsg() {
    mShowingMsg = !mShowingMsg;
}
void RenderWindow::handleInput() {
    //Camera
    mCurrentCamera->setSpeed(0.f); //cancel last frame movement
    if (mInput->F) {
        emit snapSignal();
    }
    if (mInput->CTRL) {
        if (mInput->S) {
            save();
        }
    } else if (mInput->LMB) {
        int entityID = ray->rayCast(mapFromGlobal(QCursor::pos()));
        if (entityID != -1) {
            emit rayHitEntity(entityID);
        }
    } else if (mInput->RMB) {
        if (mInput->W)
            mCurrentCamera->setSpeed(-mCameraSpeed);
        if (mInput->S)
            mCurrentCamera->setSpeed(mCameraSpeed);
        if (mInput->D)
            mCurrentCamera->moveRight(mCameraSpeed);
        if (mInput->A)
            mCurrentCamera->moveRight(-mCameraSpeed);
        if (mInput->Q)
            mCurrentCamera->updateHeight(-mCameraSpeed);
        if (mInput->E) {
            mCurrentCamera->updateHeight(mCameraSpeed);
        }
    } else if (mIsPlaying) {
        if (mInput->W)
            mMoveSys->moveZ(mLight, -mCameraSpeed);
        if (mInput->S)
            mMoveSys->moveZ(mLight, mCameraSpeed);
        if (mInput->D)
            mMoveSys->moveX(mLight, mCameraSpeed);
        if (mInput->A)
            mMoveSys->moveX(mLight, -mCameraSpeed);
        if (mInput->Q)
            mMoveSys->moveY(mLight, mCameraSpeed);
        if (mInput->E)
            mMoveSys->moveY(mLight, -mCameraSpeed);
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

void RenderWindow::mousePressEvent(QMouseEvent *event) {
    mInput->mousePressEvent(event);
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event) {
    mInput->mouseReleaseEvent(event);
}

void RenderWindow::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta() / 8;

    //if RMB, change the speed of the camera
    if (mInput->RMB) {
        if (numDegrees.y() < 1)
            setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
            setCameraSpeed(-0.001f);
    }
    event->accept();
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event) {
    if (mInput->RMB) {
        //Using mMouseXYlast as deltaXY so we don't need extra variables
        mMouseXlast = event->pos().x() - mMouseXlast;
        mMouseYlast = event->pos().y() - mMouseYlast;

        if (mMouseXlast != 0)
            mCurrentCamera->yaw(mCameraRotateSpeed * mMouseXlast);
        if (mMouseYlast != 0)
            mCurrentCamera->pitch(mCameraRotateSpeed * mMouseYlast);
    }
    mMouseXlast = event->pos().x();
    mMouseYlast = event->pos().y();
}
