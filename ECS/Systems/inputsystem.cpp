#include "inputsystem.h"
#include "camera.h"
#include "movementsystem.h"
#include "raycast.h"
#include "registry.h"
#include "renderwindow.h"
#include "resourcemanager.h"
InputSystem::InputSystem(RenderWindow *window)
    : registry(Registry::instance()), factory(ResourceManager::instance()),
      mRenderWindow(window) {
}

void InputSystem::update(DeltaTime dt) {
    if (factory->isPlaying()) {
        for (auto &camera : gameCameraControllers()) {
            if (camera->mActive)
                camera->update();
        }
    } else
        editorCamController()->update();
    handlePlayerController(dt);
    handleKeyInput();
    handleMouseInput();
}

void InputSystem::init(float aspectRatio) {
    auto view = registry->view<GameCamera>();
    for (auto entity : view) {
        mGameCameraControllers.push_back(std::make_shared<GameCameraController>(aspectRatio, entity, view.get(entity).mIsActive));
    }
}
void InputSystem::handleKeyInput() {
    if (editorInput.ESCAPE) //Shuts down whole program
    {
        emit closeEngine();
        editorInput.ESCAPE = false;
    }
    if (mPlayerController.ESCAPE) {
        factory->stop();
        mPlayerController.ESCAPE = false;
    }
    if (editorInput.F) {
        emit snapSignal();
    }
    if (editorInput.CTRL && editorInput.S) {
        factory->save();
    }
}
void InputSystem::handlePlayerController(DeltaTime dt) {
    if (factory->isPlaying()) { // No point going through this if you won't use it anyway
        gsl::Vector3D desiredVelocity(0);
        if (mPlayerController.W)
            desiredVelocity.z -= mCameraSpeed;
        if (mPlayerController.S)
            desiredVelocity.z += mCameraSpeed;
        if (mPlayerController.D)
            desiredVelocity.x += mCameraSpeed;
        if (mPlayerController.A)
            desiredVelocity.x -= mCameraSpeed;
        if (mPlayerController.Q)
            desiredVelocity.y += mCameraSpeed;
        if (mPlayerController.E)
            desiredVelocity.y -= mCameraSpeed;
        mDesiredVelocity = desiredVelocity.normalized() * mMoveSpeed * dt;
        if (mPlayer != 0) {
            registry->getSystem<MovementSystem>()->move(mPlayer, mDesiredVelocity);
        }
    }
}
void InputSystem::handleMouseInput() {
    if (editorInput.LMB) {
        int entityID = ray->mousePick(mRenderWindow->mapFromGlobal(QCursor::pos()), mRenderWindow->geometry());
        emit rayHitEntity(entityID);
    } else if (editorInput.RMB) {
        if (editorInput.W)
            mEditorCamController->moveForward(mCameraSpeed);
        if (editorInput.S)
            mEditorCamController->moveForward(-mCameraSpeed);
        if (editorInput.D)
            mEditorCamController->moveRight(mCameraSpeed);
        if (editorInput.A)
            mEditorCamController->moveRight(-mCameraSpeed);
        if (editorInput.Q)
            mEditorCamController->moveUp(-mCameraSpeed);
        if (editorInput.E) {
            mEditorCamController->moveUp(mCameraSpeed);
        }
    }
}

void InputSystem::setPlayer(const GLuint &player) {
    mPlayer = player;
}

GLuint InputSystem::player() const {
    return mPlayer;
}

Ref<CameraController> InputSystem::editorCamController() const {
    return mEditorCamController;
}

void InputSystem::setEditorCamController(const Ref<CameraController> &editorCamController) {
    mEditorCamController = editorCamController;
    ray = new Raycast(mEditorCamController);
}

std::vector<Ref<GameCameraController>> InputSystem::gameCameraControllers() const {
    return mGameCameraControllers;
}

Input &InputSystem::playerController() {
    return mPlayerController;
}
Input InputSystem::playerController() const {
    return mPlayerController;
}

void InputSystem::setCameraSpeed(float value) {
    mCameraSpeed += value;
    //Keep within min and max values
    if (mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.3f)
        mCameraSpeed = 0.3f;
}
void InputSystem::keyPressEvent(QKeyEvent *event) {
    if (factory->isPlaying())
        inputKeyPress(event, mPlayerController);
    else {
        inputKeyPress(event, editorInput);
    }
}

void InputSystem::keyReleaseEvent(QKeyEvent *event) {
    if (factory->isPlaying())
        inputKeyRelease(event, mPlayerController);
    else
        inputKeyRelease(event, editorInput);
}
void InputSystem::inputKeyPress(QKeyEvent *event, Input &input) {
    switch (event->key()) {
    case Qt::Key_L:
        input.L = true;
        break;
    case Qt::Key_F:
        input.F = true;
        break;
    case Qt::Key_W:
        input.W = true;
        break;
    case Qt::Key_A:
        input.A = true;
        break;
    case Qt::Key_S:
        input.S = true;
        break;
    case Qt::Key_D:
        input.D = true;
        break;
    case Qt::Key_Q:
        input.Q = true;
        break;
    case Qt::Key_E:
        input.E = true;
        break;
    case Qt::Key_Up:
        input.UP = true;
        break;
    case Qt::Key_Down:
        input.DOWN = true;
        break;
    case Qt::Key_Left:
        input.LEFT = true;
        break;
    case Qt::Key_Right:
        input.RIGHT = true;
        break;
    case Qt::Key_Escape:
        input.ESCAPE = true;
        break;
    case Qt::Key_Control:
        input.CTRL = true;
        break;
    default:
        break;
    }
}
void InputSystem::inputKeyRelease(QKeyEvent *event, Input &input) {
    switch (event->key()) {
    case Qt::Key_L:
        input.L = false;
        break;
    case Qt::Key_F:
        input.F = false;
        break;
    case Qt::Key_W:
        input.W = false;
        break;
    case Qt::Key_A:
        input.A = false;
        break;
    case Qt::Key_S:
        input.S = false;
        break;
    case Qt::Key_D:
        input.D = false;
        break;
    case Qt::Key_Q:
        input.Q = false;
        break;
    case Qt::Key_E:
        input.E = false;
        break;
    case Qt::Key_Up:
        input.UP = false;
        break;
    case Qt::Key_Down:
        input.DOWN = false;
        break;
    case Qt::Key_Left:
        input.LEFT = false;
        break;
    case Qt::Key_Right:
        input.RIGHT = false;
        break;
    case Qt::Key_Escape:
        input.ESCAPE = false;
        break;
    default:
        break;
    }
}
void InputSystem::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta() / 8;

    //if RMB, change the speed of the camera
    if (factory->isPlaying()) {
        if (numDegrees.y() < 1)
            setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
            setCameraSpeed(-0.001f);
    }
    event->accept();
}
void InputSystem::mouseMoveEvent(QMouseEvent *event) {
    if (editorInput.RMB) {
        mRenderWindow->setCursor(Qt::BlankCursor);
        QPoint mid = QPoint(mRenderWindow->width() / 2, mRenderWindow->height() / 2);
        QPoint glob = mRenderWindow->mapToGlobal(mid);
        QCursor::setPos(glob);
        lastPos = mid;
        if (!firstRMB) {
            GLfloat dx = GLfloat(event->x() - lastPos.x()) / mRenderWindow->width();
            GLfloat dy = GLfloat(event->y() - lastPos.y()) / mRenderWindow->height();

            if (dx != 0)
                mEditorCamController->yaw(mCameraRotateSpeed * dx);
            if (dy != 0)
                mEditorCamController->pitch(mCameraRotateSpeed * dy);
        }
        firstRMB = false;
    }
    if (!editorInput.RMB && mRenderWindow->cursor() == Qt::BlankCursor && !factory->isPlaying()) {
        mRenderWindow->setCursor(Qt::ArrowCursor);
        firstRMB = true;
    }
}
void InputSystem::onResize(float aspectRatio) {
    mEditorCamController->resize(aspectRatio);
    for (auto camera : gameCameraControllers()) {
        if (camera->mActive)
            camera->resize(aspectRatio);
    }
}
void InputSystem::inputMousePress(QMouseEvent *event, Input &input) {
    switch (event->button()) {
    case Qt::RightButton:
        input.RMB = true;
        break;
    case Qt::LeftButton:
        input.LMB = true;
        break;
    case Qt::MiddleButton:
        break;
    default:
        break;
    }
}
void InputSystem::inputMouseRelease(QMouseEvent *event, Input &input) {
    switch (event->button()) {
    case Qt::RightButton:
        input.RMB = false;
        break;
    case Qt::LeftButton:
        input.LMB = false;
        break;
    case Qt::MiddleButton:
        break;
    default:
        break;
    }
}
void InputSystem::mouseReleaseEvent(QMouseEvent *event) {
    if (factory->isPlaying())
        inputMouseRelease(event, mPlayerController);
    else
        inputMouseRelease(event, editorInput);
}
void InputSystem::mousePressEvent(QMouseEvent *event) {
    if (factory->isPlaying())
        inputMousePress(event, mPlayerController);
    else
        inputMousePress(event, editorInput);
}
