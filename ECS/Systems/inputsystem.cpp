#include "inputsystem.h"
#include "camera.h"
#include "collisionsystem.h"
#include "movementsystem.h"
#include "registry.h"
#include "renderwindow.h"
#include "resourcemanager.h"
InputSystem::InputSystem(RenderWindow *window)
    : registry{Registry::instance()}, factory{ResourceManager::instance()},
      mRenderWindow{window} {
}

void InputSystem::update(DeltaTime dt) {
    if (factory->isPlaying()) {
        for (auto &camera : mGameCameraControllers) {
            if (camera->isActive()) {
                camera->update();
                mActiveGameCamera = true;
            }
        }
        if (mIsConfined)
            confineMouseToScreen(dt);
    }
    if (!mActiveGameCamera)
        editorCamController()->update();
    handlePlayerController(dt);
    handleKeyInput();
    handleMouseInput();
}

void InputSystem::init(float aspectRatio) {
    auto view{registry->view<GameCamera>()};
    for (auto entity : view) {
        mGameCameraControllers.push_back(std::make_shared<GameCameraController>(aspectRatio, entity));
    }
}
void InputSystem::reset() {
    mEnteredWindow = false;
    mIsConfined = false;
    playerController().F1 = false;
    mIsDragging = false;
    for (auto controller : mGameCameraControllers) {
        controller->setupController();
    }
}
void InputSystem::snapToObject() {
    GLuint eID = registry->getSelectedEntity();
    MovementSystem *moveSystem{registry->system<MovementSystem>().get()};
    mEditorCamController->goTo(moveSystem->getAbsolutePosition(eID));
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
        snapToObject();
    }
    if (editorInput.CTRL && editorInput.S) {
        if (!factory->isPlaying())
            factory->save();
    }
    if (mPlayerController.F1) {
        mIsConfined = false;
        mEnteredWindow = false;
    } else if (!mIsConfined)
        mIsConfined = true;
    if (mPlayerController.L) {
        spawnTower();
        mPlayerController.L = false;
    }
}
// !!! Temporary proof of concept function, replace with makeTower or something and whatever other logic is wanted.
void InputSystem::spawnTower() {
    draggedEntity = factory->makeCube("FauxTower");
    mIsDragging = true;
}
void InputSystem::handlePlayerController(DeltaTime dt) {
    if (factory->isPlaying()) { // No point going through this if you won't use it anyway
        gsl::Vector3D desiredVelocity{0};
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
            registry->system<MovementSystem>()->move(mPlayer, mDesiredVelocity);
        }
    }
}
void InputSystem::handleMouseInput() {
    if (editorInput.LMB) {
        auto collisionSystem{registry->system<CollisionSystem>()};
        Raycast ray{collisionSystem->mousePick(mRenderWindow->mapFromGlobal(QCursor::pos()), mRenderWindow->geometry())};
        emit rayHitEntity(ray.hitEntity);
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
    if (mPlayerController.LMB) {
        if (mIsDragging) {
            mIsDragging = false;
            // Maybe add some logic here to place the object in the middle of the AABB
        }
    } else if (mPlayerController.RMB) {
        if (mIsDragging) {
            mIsDragging = false;
            registry->removeEntity(draggedEntity);
        }
    }
    if (mIsDragging)
        dragEntity(draggedEntity);
}
void InputSystem::confineMouseToScreen(DeltaTime dt) {
    int width{mRenderWindow->width()};
    int height{mRenderWindow->height()};
    QPoint pos{mRenderWindow->mapFromGlobal(QCursor::pos())};
    if (pos.x() >= 0 && pos.y() >= 0 && pos.x() <= width && pos.y() <= height) {
        mEnteredWindow = true;
    }
    if (mEnteredWindow) {
        QPoint newPos{pos};
        if (pos.x() >= width) {
            newPos.rx() = width;
            currentCameraController()->moveRight(dt);
        } else if (pos.x() <= 0) {
            newPos.rx() = 0;
            currentCameraController()->moveRight(-dt);
        }
        if (pos.y() >= height) {
            newPos.ry() = height;
            currentCameraController()->moveForward(-dt);
        } else if (pos.y() <= 0) {
            newPos.ry() = 0;
            currentCameraController()->moveForward(dt);
        }
        newPos = mRenderWindow->mapToGlobal(newPos);
        QCursor::setPos(newPos);
    }
}
void InputSystem::dragEntity(GLuint entity) {
    QPoint cursorPos{mRenderWindow->mapFromGlobal(QCursor::pos())};
    // Get the intersection point between the ray and the closest entity as a vector3d
    // make ray
    auto collisionSystem{registry->system<CollisionSystem>()};
    Raycast ray{collisionSystem->mousePick(cursorPos, mRenderWindow->geometry(), entity, 50.f)};
    Transform &tf{registry->get<Transform>(entity)};
    if (ray.hitEntity != -1) {
        // compensate for size of collider
        // some functionality might need to be added here for things like placing the entity in the center of a plane collider regardless of where on the collider the mouse is etc
        if (registry->contains<AABB>(ray.hitEntity))
            ray.hitPoint.y += registry->get<AABB>(ray.hitEntity).size.y;
        else
            ray.hitPoint.y += registry->get<Sphere>(ray.hitEntity).radius;
        ray.hitPoint.y += tf.localScale.y;
    }
    // place the object either on the mouse at 50.f distance or at correct distance for the AABB hit
    tf.localPosition = ray.hitPoint;
    tf.matrixOutdated = true;
}
void InputSystem::setPlayer(const GLuint &player) {
    mPlayer = player;
}

void InputSystem::setGameCameraInactive() {
    mActiveGameCamera = false;
}

GLuint InputSystem::player() const {
    return mPlayer;
}

Ref<CameraController> InputSystem::editorCamController() const {
    return mEditorCamController;
}

void InputSystem::setEditorCamController(const Ref<CameraController> &editorCamController) {
    mEditorCamController = editorCamController;
}

std::vector<Ref<GameCameraController>> InputSystem::gameCameraControllers() const {
    return mGameCameraControllers;
}

Ref<CameraController> InputSystem::currentCameraController() {
    if (factory->isPlaying()) {
        for (auto controller : mGameCameraControllers) {
            if (controller->isActive()) {
                return controller;
            }
        }
    }
    return mEditorCamController;
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
    if (factory->isPlaying() && mActiveGameCamera)
        inputKeyPress(event, mPlayerController);
    if (!mActiveGameCamera) {
        inputKeyPress(event, editorInput);
    }
}

void InputSystem::keyReleaseEvent(QKeyEvent *event) {
    if (factory->isPlaying() && mActiveGameCamera)
        inputKeyRelease(event, mPlayerController);
    if (!mActiveGameCamera)
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
    case Qt::Key_F1: // toggle here
        input.F1 = !input.F1;
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
    QPoint numDegrees{event->angleDelta() / 8};

    //if RMB, change the speed of the camera
    if (!mActiveGameCamera) {
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
        QPoint mid{QPoint(mRenderWindow->width() / 2, mRenderWindow->height() / 2)};
        QPoint glob{mRenderWindow->mapToGlobal(mid)};
        QCursor::setPos(glob);
        lastPos = mid;
        if (!firstRMB) {
            GLfloat dx{GLfloat(event->x() - lastPos.x()) / mRenderWindow->width()};
            GLfloat dy{GLfloat(event->y() - lastPos.y()) / mRenderWindow->height()};

            if (dx != 0)
                mEditorCamController->yaw(mCameraRotateSpeed * dx);
            if (dy != 0)
                mEditorCamController->pitch(mCameraRotateSpeed * dy);
        }
        firstRMB = false;
    }
    if (!editorInput.RMB && mRenderWindow->cursor() == Qt::BlankCursor && !mActiveGameCamera) {
        mRenderWindow->setCursor(Qt::ArrowCursor);
        firstRMB = true;
    }
}
void InputSystem::onResize(float aspectRatio) {
    mEditorCamController->resize(aspectRatio);
    for (auto camera : mGameCameraControllers) {
        if (camera->isActive())
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
    if (factory->isPlaying() && mActiveGameCamera)
        inputMouseRelease(event, mPlayerController);
    if (!mActiveGameCamera)
        inputMouseRelease(event, editorInput);
}
void InputSystem::mousePressEvent(QMouseEvent *event) {
    if (factory->isPlaying() && mActiveGameCamera)
        inputMousePress(event, mPlayerController);
    if (!mActiveGameCamera)
        inputMousePress(event, editorInput);
}
void InputSystem::setCameraPositionX(double xIn) {
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mCameraPosition.x = xIn;
    cam.mOutDated = true;
}
void InputSystem::setCameraPositionY(double yIn) {
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mCameraPosition.y = yIn;
    cam.mOutDated = true;
}
void InputSystem::setCameraPositionZ(double zIn) {
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mCameraPosition.z = zIn;
    cam.mOutDated = true;
}
void InputSystem::setYaw(double yaw) {
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mYaw = yaw;
    cam.mOutDated = true;
}
void InputSystem::setActiveCamera(bool checked) {
    GLuint entityID{registry->getSelectedEntity()};
    auto view{registry->view<GameCamera>()};
    GameCamera &selectedCam{view.get(entityID)};
    if (checked) {
        selectedCam.mIsActive = true;
        selectedCam.mOutDated = true;
        for (GLuint camID : view) {
            if (camID == entityID)
                continue;
            auto &cam = view.get(camID);
            cam.mIsActive = false;
            cam.mOutDated = true;
        }
        if (factory->isPlaying())
            factory->setActiveCameraController(currentCameraController());
    } else {
        selectedCam.mIsActive = false;
        selectedCam.mOutDated = true;
        setGameCameraInactive();
        // Unchecking a checked box defaults the editor to the main editor camera controller
        factory->setActiveCameraController(editorCamController());
    }
}
void InputSystem::setPitch(double pitch) {
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mPitch = pitch;
    cam.mOutDated = true;
}
