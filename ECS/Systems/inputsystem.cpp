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
        gameCameraController()->update();
    } else
        editorCamController()->update();
    handlePlayerController(dt);
    handleKeyInput();
    handleMouseInput();
}
void InputSystem::handleKeyInput() {
    if (editorInput.ESCAPE || registry->get<Input>(mPlayerController).ESCAPE) //Shuts down whole program
    {
        emit closeEngine();
        editorInput.ESCAPE = false;
    }
    if (editorInput.F) {
        emit snapSignal();
        return;
    }
}
void InputSystem::handlePlayerController(float deltaTime) {
    if (factory->isPlaying()) { // No point going through this if you won't use it anyway
        Input &player = registry->get<Input>(mPlayerController);
        gsl::Vector3D desiredVelocity(0);

        if (player.W)
            desiredVelocity.z -= mCameraSpeed;
        if (player.S)
            desiredVelocity.z += mCameraSpeed;
        if (player.D)
            desiredVelocity.x += mCameraSpeed;
        if (player.A)
            desiredVelocity.x -= mCameraSpeed;
        if (player.Q)
            desiredVelocity.y += mCameraSpeed;
        if (player.E)
            desiredVelocity.y -= mCameraSpeed;
        registry->getSystem<MovementSystem>()->move(mPlayerController, desiredVelocity.normalized() * mMoveSpeed * deltaTime);
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

Ref<CameraController> InputSystem::editorCamController() const {
    return mEditorCamController;
}

Ref<GameCameraController> InputSystem::gameCameraController() const {
    return mGameCameraController;
}

void InputSystem::setGameCameraController(const Ref<GameCameraController> &gameCameraController, GLuint gameCamMeshID) {
    mGameCameraController = gameCameraController;
    mGameCameraController->setMeshID(gameCamMeshID);

    mGameCameraController->update();
}

void InputSystem::setEditorCamController(const Ref<CameraController> &editorCamController) {
    mEditorCamController = editorCamController;
    ray = new Raycast(mEditorCamController);
}

GLuint InputSystem::playerController() const {
    return mPlayerController;
}

void InputSystem::setPlayerController(const GLuint &playerController) {
    mPlayerController = playerController;
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
    if (event->modifiers() == Qt::Key_Control && event->key() == Qt::Key_S) {
        factory->save();
        return;
    }
    if (factory->isPlaying())
        inputKeyPress(event, registry->get<Input>(mPlayerController));
    else {
        inputKeyPress(event, editorInput);
    }
}

void InputSystem::keyReleaseEvent(QKeyEvent *event) {
    if (factory->isPlaying())
        inputKeyRelease(event, registry->get<Input>(mPlayerController));
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
    mGameCameraController->resize(aspectRatio);
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
        inputMouseRelease(event, registry->get<Input>(mPlayerController));
    else
        inputMouseRelease(event, editorInput);
}
void InputSystem::mousePressEvent(QMouseEvent *event) {
    if (factory->isPlaying())
        inputMousePress(event, registry->get<Input>(mPlayerController));
    else
        inputMousePress(event, editorInput);
}
