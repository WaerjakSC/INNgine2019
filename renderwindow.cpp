#include "renderwindow.h"
#include "Components/inputcomponent.h"
#include "innpch.h"
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QStatusBar>
#include <QTimer>
#include <chrono>

#include "soundmanager.h"
#include "soundsource.h"
#include <iostream>
#include <thread> //for sleep_for

#include "mainwindow.h"

#include "Assets/Meshes/trianglesurface.h"
#include "Components/meshcomponent.h"
#include "Shaders/colorshader.h"
#include "Shaders/phongshader.h"
#include "Shaders/textureshader.h"
#include "Systems/movementsystem.h"
#include "Systems/rendersystem.h"
#include "Views/renderview.h"
#include "lightobject.h"
typedef gsl::Vector3D vec3;

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false),
      mFactory(ResourceManager::instance()), mSoundManager(SoundManager::instance()),
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
    mInput = new InputComponent();
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
    mFactory->LoadShader(Color);
    mFactory->LoadShader(Tex);
    mFactory->LoadShader(Phong);

    //**********************  Texture stuff: **********************

    mFactory->LoadTexture("white.bmp");
    mFactory->LoadTexture("hund.bmp", 1);
    mFactory->LoadTexture("skybox.bmp", 2);

    //Set the textures loaded to a texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mFactory->GetTexture("white.bmp")->id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mFactory->GetTexture("hund.bmp")->id());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mFactory->GetTexture("skybox.bmp")->id());

    //********************** Making the objects to be drawn **********************
    mFactory->makeXYZ();
    GLuint skybox = mFactory->makeSkyBox();
    GLuint billboard = mFactory->makeBillBoard();
    mLight = mFactory->makeLightObject();

    GLuint boxID = mFactory->makeTriangleSurface("box2.txt");
    static_cast<MaterialComponent *>(mFactory->getComponent(Material, boxID))->setShader(Color);
    //one gnome
    GLuint gnome = mFactory->make3DObject("gnome.obj", Phong); // Simple creation of item by using factory
    mFactory->setParent(gnome, boxID);

    //********************** Set up camera **********************
    mCurrentCamera = new Camera();
    mCurrentCamera->setPosition(vec3(1.f, 1.f, 4.4f));
    //    mCurrentCamera->yaw(45.f);
    //    mCurrentCamera->pitch(5.f);

    //new system - shader sends uniforms so needs to get the view and projection matrixes from camera
    mFactory->GetShader(ShaderType::Color)->setCurrentCamera(mCurrentCamera);
    mFactory->GetShader(ShaderType::Tex)->setCurrentCamera(mCurrentCamera);
    mFactory->GetShader(ShaderType::Phong)->setCurrentCamera(mCurrentCamera);

    // Set up the systems.
    mRenderer = new RenderSystem(mFactory->getShaders());
    mMoveSys = new MovementSystem(&mFactory->getTransforms());

    // Add game objects to the scene hierarchy GUI
    mMainWindow->insertGameObjects(mFactory->getGameObjectIndex());

    // Initial positional setup.
    mMoveSys->setPosition(gnome, vec3(1.3f, 1.3f, -3.5f));
    mMoveSys->setPosition(mLight, 2.5f, 3.f, 0.f);
    mMoveSys->setRotation(mLight, vec3(0.0f, 180.f, 0.0f));
    mMoveSys->setPosition(billboard, vec3(4.f, 0.f, -3.5f));
    mMoveSys->setScale(skybox, 15);
    mMoveSys->setScale(gnome, vec3(0.1f, 0.1f, 0.1f));
    mMoveSys->setPosition(boxID, vec3(-3.3f, .3f, -3.5f));

    // Set up connections between MainWindow options and related systems.
    connect(mMainWindow, &MainWindow::made3DObject, mFactory->getRenderView(), &RenderView::addEntity);
    connect(mFactory->getRenderView(), &RenderView::updateSystem, mRenderer, &RenderSystem::newEntity);

    // -----------Sound test-----------
    //Some sounds...

    //loads the sounds
    //Vector - placement - no effect on stereo sound
    //parameters:
    //createSource(std::string name, Vector3 pos, std::string filePath, bool loop, float gain)

    /*)
    mStereoSound = mSoundManager->createSource(
                "Stereo", Vector3(0.0f, 0.0f, 0.0f),
                "../INNgine2019/Assets/stereo.wav", true, 1.0f);



    mLaserSound = mSoundManager->createSource(
                "Laser", Vector3(20.0f, 0.0f, 0.0f),
                "../Sound/Assets/laser.wav", false, 0.7f);


    mSong = mSoundManager->createSource(
                "Caravan", Vector3(0.0f, 0.0f, 0.0f),
                "../Sound/Assets/Caravan_mono.wav", false, 1.0f);
               */

    mStereoSound = mSoundManager->createSource(
        "Explosion", Vector3(0.0f, 0.0f, 0.0f),
        "../INNgine2019/Assets/explosion.wav", false, 1.0f);
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

    mMoveSys->update();
    mRenderer->render();
    mSoundManager->updateListener(mCurrentCamera->position(), gsl::Vector3D(1), mCurrentCamera->forward(), mCurrentCamera->up());

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

void RenderWindow::updateScene() {
    mFactory->getGameObjects();
}
RenderSystem *RenderWindow::renderer() const {
    return mRenderer;
}
MovementSystem *RenderWindow::movement() const {
    return mMoveSys;
}
ResourceManager *RenderWindow::factory() const {
    return mFactory;
}

SoundManager *RenderWindow::soundManager() const {
    return mSoundManager;
}
void RenderWindow::soundTest() {
    //plays the sounds
    mStereoSound->play();
    mStereoSound->setPosition(Vector3(0.0f, 0.0f, 0.0f));
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
            //showing some statistics in status bar
            mMainWindow->statusBar()->showMessage(" Time pr FrameDraw: " +
                                                  QString::number(nsecElapsed / 1000000., 'g', 4) + " ms  |  " +
                                                  "FPS (approximated): " + QString::number(1E9 / nsecElapsed, 'g', 7));
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
void RenderWindow::setCameraSpeed(float value) {
    mCameraSpeed += value;
    //Keep within min and max values
    if (mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.3f)
        mCameraSpeed = 0.3f;
}

void RenderWindow::handleInput() {
    //Camera
    mCurrentCamera->setSpeed(0.f); //cancel last frame movement
    if (mInput->L) {
        soundTest();
    }
    if (mInput->RMB) {
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
            //Must cleanly shut down the soundmanager
            SoundManager::instance()->cleanUp();
        }
    } else { // Doesn't work atm, need to fix transformcomponents and stuff
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
