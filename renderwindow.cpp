#include "renderwindow.h"
#include "innpch.h"
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QStatusBar>
#include <QTimer>
#include <chrono>

#include "mainwindow.h"

#include "Assets/Meshes/trianglesurface.h"
#include "Components/meshcomponent.h"
#include "Shaders/colorshader.h"
#include "Shaders/phongshader.h"
#include "Shaders/textureshader.h"
#include "lightobject.h"
#include "resourcemanager.h"

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false), mMainWindow(mainWindow) {
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
    for (int i = 0; i < 4; ++i) {
        //if (mShaderProgram[i])
        //    delete mShaderProgram[i];
    }
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

    // Create Resource Manager instance
    ResourceManager &factory = ResourceManager::instance();
    //Compile shaders:
    factory.LoadShader(ShaderType::Color);
    factory.LoadShader(ShaderType::Tex);
    factory.LoadShader(ShaderType::Phong);

    //**********************  Texture stuff: **********************

    factory.LoadTexture("white.bmp");
    factory.LoadTexture("hund.bmp", 1);
    factory.LoadTexture("skybox.bmp", 2);

    //Set the textures loaded to a texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, factory.GetTexture("white.bmp")->id());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, factory.GetTexture("hund.bmp")->id());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, factory.GetTexture("skybox.bmp")->id());

    //********************** Making the objects to be drawn **********************
    // Nothing will render for now, but at least the factory can make the objects.
    factory.makeXYZ();

    factory.makeSkyBox();

    //    temp = new OctahedronBall(2);
    //    temp->init();
    //    temp->setShader(mShaderProgram[0]);
    //    temp->mMatrix.scale(0.5f, 0.5f, 0.5f);
    //    temp->mName = "Ball";
    //    mVisualObjects.push_back(temp);
    //    mPlayer = temp;

    //    temp = new GameObject("Cube");
    //    temp->addComponent(new SkyBox());
    //    temp->init();
    //    temp->setShaders(ResourceManager::GetShader(ShaderType::Tex));
    //    temp->mMatrix.scale(15.f);
    //    mGameObjects.emplace_back(temp);

    factory.makeBillBoard();
    //    temp->addComponent(new BillBoard());
    //    temp->init();
    //    temp->setShaders(ResourceManager::GetShader(ShaderType::Tex));
    //    temp->mMatrix.translate(4.f, 0.f, -3.5f);
    //    //    temp->mRenderWindow = this; // Not sure if needed
    //    mGameObjects.emplace_back(temp);

    factory.makeLightObject();
    //    mLight = new Light("Light");
    //    temp = mLight;
    //    temp->init();
    //    temp->setShaders(ResourceManager::GetShader(ShaderType::Tex));
    //    temp->mMatrix.translate(2.5f, 3.f, 0.f);
    //    temp->mMatrix.rotateY(180.f);
    //    temp->mRenderWindow = this;

    dynamic_cast<PhongShader *>(ResourceManager::GetShader("phongshader"))->setLight(mLight);

    //testing triangle surface class
    GLuint boxID = factory.makeTriangleSurface("box2.txt");
    dynamic_cast<MaterialComponent *>(factory.getComponent(Material, boxID))->setShader(factory.GetShader(Color));
    //    temp = new GameObject("TriangleSurface");
    //    auto *tempMesh = new MeshComponent(ResourceManager::LoadMesh("box2.txt"));
    //    temp->addComponent(tempMesh);
    //    temp->init();
    //    temp->mMatrix.rotateY(180.f);
    //    temp->setShaders(ResourceManager::GetShader(ShaderType::Color));
    //    mGameObjects.emplace_back(temp);

    //one monkey
    factory.makeGameObject("Monkey");
    factory.addMeshComponent("monkey.obj"); // Showing that you can access the last created gameobject without using an eID
    factory.addComponent(Material);         // To-do: Make unique functions for each type of component for more precise creation
    dynamic_cast<MaterialComponent *>(factory.getComponent(Material))->setShader(factory.GetShader(Phong));
    //    temp->mMatrix.scale(0.5f);
    //    temp->mMatrix.translate(3.f, 2.f, -2.f);
    //    mGameObjects.emplace_back(temp);

    //    int x{0};
    //    int y{0};
    //    int numberOfObjs{100};
    //    for (int i{0}; i < numberOfObjs; i++) {
    //        temp = new GameObject("Monkey");
    //        temp->addComponent(new MeshComponent(ResourceManager::LoadMesh("monkey.obj")));
    //        temp->setShaders(ResourceManager::GetShader(ShaderType::Phong));
    //        temp->init();
    //        x++;
    //        temp->mMatrix.translate(0.f + x, 0.f, -2.f - y);
    //        temp->mMatrix.scale(0.5f);
    //        mGameObjects.push_back(temp);
    //        if (x % 10 == 0) {
    //            x = 0;
    //            y++;
    //        }
    //    }
    // End of performance test
    //********************** Set up camera **********************
    mCurrentCamera = new Camera();
    mCurrentCamera->setPosition(gsl::Vector3D(1.f, 1.f, 4.4f));
    //    mCurrentCamera->yaw(45.f);
    //    mCurrentCamera->pitch(5.f);

    //new system - shader sends uniforms so needs to get the view and projection matrixes from camera

    factory.GetShader(ShaderType::Color)->setCurrentCamera(mCurrentCamera);
    factory.GetShader(ShaderType::Tex)->setCurrentCamera(mCurrentCamera);
    factory.GetShader(ShaderType::Phong)->setCurrentCamera(mCurrentCamera);
}

///Called each frame - doing the rendering
void RenderWindow::render() {
    //calculate the time since last render-call
    //this should be the same as xxx in the mRenderTimer->start(xxx) set in RenderWindow::exposeEvent(...)
    //    auto now = std::chrono::high_resolution_clock::now();
    //    std::chrono::duration<float> duration = now - mLastTime;
    //    std::cout << "Chrono deltaTime " << duration.count()*1000 << " ms" << std::endl;
    //    mLastTime = now;

    //input
    handleInput();

    mCurrentCamera->update();

    mTimeStart.restart();        //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    //to clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto visObject : mGameObjects) {
        visObject->update();
        //        checkForGLerrors();
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
    //    auto end = std::chrono::high_resolution_clock::now();
    //    std::chrono::duration<float> duration = end - start;
    //    std::cout << "Chrono deltaTime " << duration.count()*1000 << " ms" << std::endl;

    //    calculateFramerate();
}

//void RenderWindow::setupPlainShader() {
//    mMatrixUniform0 = glGetUniformLocation(ResourceManager::GetShader(ShaderType::Color)->getProgram(), "mMatrix");
//    vMatrixUniform0 = glGetUniformLocation(ResourceManager::GetShader(ShaderType::Color)->getProgram(), "vMatrix");
//    pMatrixUniform0 = glGetUniformLocation(ResourceManager::GetShader(ShaderType::Color)->getProgram(), "pMatrix");
//}

//void RenderWindow::setupTextureShader() {
//    mMatrixUniform1 = glGetUniformLocation(ResourceManager::GetShader(ShaderType::Tex)->getProgram(), "mMatrix");
//    vMatrixUniform1 = glGetUniformLocation(ResourceManager::GetShader(ShaderType::Tex)->getProgram(), "vMatrix");
//    pMatrixUniform1 = glGetUniformLocation(ResourceManager::GetShader(ShaderType::Tex)->getProgram(), "pMatrix");
//    mTextureUniform = glGetUniformLocation(ResourceManager::GetShader(ShaderType::Tex)->getProgram(), "textureSampler");
//}

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
    if (mInput.RMB) {
        if (mInput.W)
            mCurrentCamera->setSpeed(-mCameraSpeed);
        if (mInput.S)
            mCurrentCamera->setSpeed(mCameraSpeed);
        if (mInput.D)
            mCurrentCamera->moveRight(mCameraSpeed);
        if (mInput.A)
            mCurrentCamera->moveRight(-mCameraSpeed);
        if (mInput.Q)
            mCurrentCamera->updateHeigth(-mCameraSpeed);
        if (mInput.E)
            mCurrentCamera->updateHeigth(mCameraSpeed);
    } else {
        if (mInput.W)
            mLight->mMatrix.translateZ(-mCameraSpeed);
        if (mInput.S)
            mLight->mMatrix.translateZ(mCameraSpeed);
        if (mInput.D)
            mLight->mMatrix.translateX(mCameraSpeed);
        if (mInput.A)
            mLight->mMatrix.translateX(-mCameraSpeed);
        if (mInput.Q)
            mLight->mMatrix.translateY(mCameraSpeed);
        if (mInput.E)
            mLight->mMatrix.translateY(-mCameraSpeed);
    }
}

void RenderWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) //Shuts down whole program
    {
        mMainWindow->close();
    }

    //    You get the keyboard input like this
    if (event->key() == Qt::Key_W) {
        mInput.W = true;
    }
    if (event->key() == Qt::Key_S) {
        mInput.S = true;
    }
    if (event->key() == Qt::Key_D) {
        mInput.D = true;
    }
    if (event->key() == Qt::Key_A) {
        mInput.A = true;
    }
    if (event->key() == Qt::Key_Q) {
        mInput.Q = true;
    }
    if (event->key() == Qt::Key_E) {
        mInput.E = true;
    }
    if (event->key() == Qt::Key_Z) {
    }
    if (event->key() == Qt::Key_X) {
    }
    if (event->key() == Qt::Key_Up) {
        mInput.UP = true;
    }
    if (event->key() == Qt::Key_Down) {
        mInput.DOWN = true;
    }
    if (event->key() == Qt::Key_Left) {
        mInput.LEFT = true;
    }
    if (event->key() == Qt::Key_Right) {
        mInput.RIGHT = true;
    }
    if (event->key() == Qt::Key_U) {
    }
    if (event->key() == Qt::Key_O) {
    }
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W) {
        mInput.W = false;
    }
    if (event->key() == Qt::Key_S) {
        mInput.S = false;
    }
    if (event->key() == Qt::Key_D) {
        mInput.D = false;
    }
    if (event->key() == Qt::Key_A) {
        mInput.A = false;
    }
    if (event->key() == Qt::Key_Q) {
        mInput.Q = false;
    }
    if (event->key() == Qt::Key_E) {
        mInput.E = false;
    }
    if (event->key() == Qt::Key_Z) {
    }
    if (event->key() == Qt::Key_X) {
    }
    if (event->key() == Qt::Key_Up) {
        mInput.UP = false;
    }
    if (event->key() == Qt::Key_Down) {
        mInput.DOWN = false;
    }
    if (event->key() == Qt::Key_Left) {
        mInput.LEFT = false;
    }
    if (event->key() == Qt::Key_Right) {
        mInput.RIGHT = false;
    }
    if (event->key() == Qt::Key_U) {
    }
    if (event->key() == Qt::Key_O) {
    }
}

void RenderWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton)
        mInput.RMB = true;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = true;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = true;
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton)
        mInput.RMB = false;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = false;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = false;
}

void RenderWindow::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta() / 8;

    //if RMB, change the speed of the camera
    if (mInput.RMB) {
        if (numDegrees.y() < 1)
            setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
            setCameraSpeed(-0.001f);
    }
    event->accept();
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event) {
    if (mInput.RMB) {
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
