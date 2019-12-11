#include "inputsystem.h"
#include "camera.h"
#include "cameracontroller.h"
#include "collisionsystem.h"
#include "movementsystem.h"
#include "registry.h"
#include "renderwindow.h"
#include "resourcemanager.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

InputSystem::InputSystem(RenderWindow *window, cjk::Ref<CameraController> editorController)
    : registry{Registry::instance()}, factory{ResourceManager::instance()},
      mRenderWindow{window}, mEditorCamController(editorController)
{
}

void InputSystem::update(DeltaTime dt)
{
    if (!mActiveGameCamera)
        mEditorCamController->update();
    handlePlayerController(dt);
    handleKeyInput();
    handleMouseInput();
}

void InputSystem::updatePlayOnly(DeltaTime deltaTime)
{
    for (auto &camera : mGameCameraControllers) {
        if (camera->isActive()) {
            camera->update();
            mActiveGameCamera = true;
        }
    }
    if (mIsConfined)
        confineMouseToScreen(deltaTime);
}

void InputSystem::init(float aspectRatio)
{
    auto view{registry->view<GameCamera>()};
    for (auto entity : view) {
        mGameCameraControllers.push_back(std::make_shared<GameCameraController>(aspectRatio, entity));
    }
}
void InputSystem::reset()
{
    mEnteredWindow = false;
    mIsConfined = false;
    mPlayerController.F1 = false;
    mIsDragging = false;
    for (auto controller : mGameCameraControllers) {
        controller->setupController();
    }
}
void InputSystem::snapToObject()
{
    GLuint eID{registry->getSelectedEntity()};
    if (registry->contains<Transform>(eID)) {
        MovementSystem *moveSystem{registry->system<MovementSystem>().get()};
        mEditorCamController->goTo(moveSystem->getAbsolutePosition(eID));
    }
}
void InputSystem::handleKeyInput()
{
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
    if (editorInput.H) {
        GLuint eID{registry->getSelectedEntity()};
        if (eID != 0) {
            emit toggleRendered(eID);
            editorInput.H = false;
        }
    }
    if (mPlayerController.F1) {
        mIsConfined = false;
        mEnteredWindow = false;
    }
    else if (!mIsConfined)
        mIsConfined = true;
    if (mPlayerController.L) {
        if (!mIsDragging) {
            spawnTower();
        }
        mPlayerController.L = false;
    }
}
// !!! Temporary proof of concept function, replace with makeTower or something and whatever other logic is wanted.
void InputSystem::spawnTower()
{
    draggedEntity = factory->makeTower("Tower");
    mIsDragging = true;
    setPlaneColors(mIsDragging);
    // Might want to also "pause" the game here, or rather stop all AI and Movement while still allowing the player to move the camera and place the tower.
}
void InputSystem::setPlaneColors(bool dragMode)
{
    auto view{registry->view<Buildable, Material>()};
    if (dragMode) {
        for (auto entity : view) {
            auto [build, mat]{view.get<Buildable, Material>(entity)};
            // set color according to buildable state -- Red means unbuildable, green means buildable.
            if (build.isBuildable) {
                mat.objectColor = green * 0.8f; // GREEN'ish
            }
            else {
                mat.objectColor = red * 0.8f; // RED'ish
            }
        }
    }
    else { // set colors back to the original and set shader back to Phong for lighting
        for (auto entity : view) {
            auto &mat{view.get<Material>(entity)};
            mat.objectColor = origColor;
        }
    }
}
void InputSystem::handlePlayerController([[maybe_unused]] DeltaTime dt)
{
    if (factory->isPlaying()) { // We don't have a movable character anyway, disabling this.
    }
}
void InputSystem::handleMouseInput()
{
    if (editorInput.LMB) {
        auto collisionSystem{registry->system<CollisionSystem>()};
        Raycast ray{collisionSystem->mousePick(mRenderWindow->mapFromGlobal(QCursor::pos()), mRenderWindow->geometry())};
        emit rayHitEntity(ray.hitEntity);
    }
    else if (editorInput.RMB) {
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
            placeTower();
        }
    }
    else if (mPlayerController.RMB) {
        if (mIsDragging) {
            mIsDragging = false;
            // reset the colors after discarding the tower.
            setPlaneColors(mIsDragging);
            // remove the dragged entity from play - discarded by the player.
            registry->removeEntity(draggedEntity);
        }
    }
    if (mIsDragging)
        dragEntity(draggedEntity);
}

bool InputSystem::buildableDebugMode() const
{
    return mBuildableDebug;
}

cjk::Ref<CameraController> InputSystem::editorCamController() const
{
    return mEditorCamController;
}
void InputSystem::confineMouseToScreen(DeltaTime dt)
{
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
        }
        else if (pos.x() <= 0) {
            newPos.rx() = 0;
            currentCameraController()->moveRight(-dt);
        }
        if (pos.y() >= height) {
            newPos.ry() = height;
            currentCameraController()->moveForward(-dt);
        }
        else if (pos.y() <= 0) {
            newPos.ry() = 0;
            currentCameraController()->moveForward(dt);
        }
        newPos = mRenderWindow->mapToGlobal(newPos);
        QCursor::setPos(newPos);
    }
}
void InputSystem::setBuildableObject(bool state)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto view{registry->view<Buildable>()};
    Buildable &build{view.get(entityID)};
    build.isBuildable = state;
    updateBuildable(entityID);
}
void InputSystem::updateBuildable(GLuint entityID)
{
    auto view{registry->view<Transform, Buildable, Material, Mesh, AABB>()};
    auto [trans, build, mat, mesh, aabb]{view.get<Transform, Buildable, Material, Mesh, AABB>(entityID)};
    if (build.isBuildable) {
        if (mIsDragging || mBuildableDebug) {
            mat.objectColor = green * 0.8f;
        }
        mesh = factory->getMesh("cube.obj"); // replace with a cube type mesh as wall ingame
        float scaleY{trans.localScale.x / 2.f};
        trans.localScale.y = scaleY; // set local scale in y axis to be similar to a raised platform
        aabb.size.y = scaleY;        // update the AABB to keep up with mesh scale.
        trans.localPosition.y += scaleY;
        trans.matrixOutdated = true;
    }
    else {
        if (mIsDragging || mBuildableDebug) {
            mat.objectColor = red * 0.8f;
        }
        mesh = factory->getMesh("Plane");
        trans.localPosition.y = 0.f;
        float nScale{0.01f};
        trans.localScale.y = nScale;
        aabb.size.y = nScale;
        trans.matrixOutdated = true;
    }
}
void InputSystem::placeTower()
{
    auto view{registry->view<Transform, Buildable>()};
    if (view.contains(lastHitEntity)) {
        auto &build{view.get<Buildable>(lastHitEntity)};
        if (build.isBuildable) {
            mIsDragging = false;
            build.isBuildable = false;
            auto draggedView{registry->view<Transform, AABB, TowerComponent>()};
            auto [towerTransform, aabb, tower]{draggedView.get<Transform, AABB, TowerComponent>(draggedEntity)};
            tower.state = TowerStates::IDLE;
            auto &planeTransform{view.get<Transform>(lastHitEntity)};
            float scaleY{planeTransform.localScale.y};
            vec3 topCenterOfTarget{planeTransform.localPosition};
            topCenterOfTarget.y += (aabb.size.y + scaleY); // place the tower just slightly above the Plane it's sitting on to avoid clipping.
            towerTransform.localPosition = topCenterOfTarget;
            towerTransform.matrixOutdated = true;
            setPlaneColors(mIsDragging);
        }
    }
}
void InputSystem::setBuildableDebug(bool value)
{
    mBuildableDebug = value;
}
void InputSystem::dragEntity(GLuint entity)
{
    QPoint cursorPos{mRenderWindow->mapFromGlobal(QCursor::pos())};
    // Get the intersection point between the ray and the closest entity as a vector3d
    // make ray
    auto collisionSystem{registry->system<CollisionSystem>()};
    // we pass the dragged entity to mousePick in order to ignore that entity in raycasting
    Raycast ray{collisionSystem->mousePick(cursorPos, mRenderWindow->geometry(), entity, 100.f)};

    Transform &tf{registry->get<Transform>(entity)};

    // some functionality might need to be added here for things like placing the entity in the center of a plane collider regardless of where on the collider the mouse is etc
    if (registry->contains<AABB>(ray.hitEntity)) { // We only want to deal with AABB colliders
        auto view{registry->view<Buildable, Material>()};
        if (view.contains(lastHitEntity) && static_cast<int>(lastHitEntity) != ray.hitEntity) { // reset the color after hitting another object
            auto &mat = view.get<Material>(lastHitEntity);
            if (view.get<Buildable>(lastHitEntity).isBuildable) {
                mat.objectColor = green * 0.8f;
            }
            else
                mat.objectColor = red * 0.8f;
        }
        // compensate for size of collider
        float colliderHeight{registry->get<AABB>(ray.hitEntity).size.y};
        ray.hitPoint.y += colliderHeight * 2;
        if (view.contains(ray.hitEntity)) {
            auto &mat = view.get<Material>(ray.hitEntity);
            lastHitEntity = ray.hitEntity;
            if (view.get<Buildable>(ray.hitEntity).isBuildable) {
                mat.objectColor = green;
            }
            else
                mat.objectColor = red; // slightly increase the color intensity of the object as it's being moused over.
        }
    }
    ray.hitPoint.y += tf.localScale.y;

    // place the object either on the mouse at 100.f distance or at correct distance for the AABB hit
    tf.localPosition = ray.hitPoint;
    tf.matrixOutdated = true;
}

std::vector<cjk::Ref<GameCameraController>> InputSystem::gameCameraControllers() const
{
    return mGameCameraControllers;
}

cjk::Ref<CameraController> InputSystem::currentCameraController()
{
    if (factory->isPlaying()) {
        for (auto controller : mGameCameraControllers) {
            if (controller->isActive()) {
                return controller;
            }
        }
    }
    return mEditorCamController;
}
void InputSystem::setGameCameraInactive()
{
    mActiveGameCamera = false;
}
void InputSystem::setCameraSpeed(float value)
{
    mCameraSpeed += value;
    //Keep within min and max values
    if (mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.3f)
        mCameraSpeed = 0.3f;
}
void InputSystem::keyPressEvent(QKeyEvent *event)
{
    if (factory->isPlaying() && mActiveGameCamera)
        inputKeyPress(event, mPlayerController);
    if (!mActiveGameCamera) {
        inputKeyPress(event, editorInput);
    }
}

void InputSystem::keyReleaseEvent(QKeyEvent *event)
{
    if (factory->isPlaying() && mActiveGameCamera)
        inputKeyRelease(event, mPlayerController);
    if (!mActiveGameCamera)
        inputKeyRelease(event, editorInput);
}
void InputSystem::inputKeyPress(QKeyEvent *event, Input &input)
{
    switch (event->key()) {
    case Qt::Key_L:
        input.L = true;
        break;
    case Qt::Key_H:
        input.H = true;
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
void InputSystem::inputKeyRelease(QKeyEvent *event, Input &input)
{
    switch (event->key()) {
    case Qt::Key_L:
        input.L = false;
        break;
    case Qt::Key_H:
        input.H = false;
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
void InputSystem::wheelEvent(QWheelEvent *event)
{
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

void InputSystem::mouseMoveEvent(QMouseEvent *event)
{
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
void InputSystem::onResize(float aspectRatio)
{
    mEditorCamController->resize(aspectRatio);
    for (auto camera : mGameCameraControllers) {
        if (camera->isActive())
            camera->resize(aspectRatio);
    }
}
void InputSystem::inputMousePress(QMouseEvent *event, Input &input)
{
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
void InputSystem::inputMouseRelease(QMouseEvent *event, Input &input)
{
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
void InputSystem::mouseReleaseEvent(QMouseEvent *event)
{
    if (factory->isPlaying() && mActiveGameCamera)
        inputMouseRelease(event, mPlayerController);
    if (!mActiveGameCamera)
        inputMouseRelease(event, editorInput);
}
void InputSystem::mousePressEvent(QMouseEvent *event)
{
    if (factory->isPlaying() && mActiveGameCamera)
        inputMousePress(event, mPlayerController);
    if (!mActiveGameCamera)
        inputMousePress(event, editorInput);
}
void InputSystem::setCameraPositionX(double xIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mCameraPosition.x = xIn;
    cam.mOutDated = true;
}
void InputSystem::setCameraPositionY(double yIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mCameraPosition.y = yIn;
    cam.mOutDated = true;
}
void InputSystem::setCameraPositionZ(double zIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mCameraPosition.z = zIn;
    cam.mOutDated = true;
}
void InputSystem::setYaw(double yaw)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mYaw = yaw;
    cam.mOutDated = true;
}
void InputSystem::setActiveCamera(bool checked)
{
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
    }
    else {
        selectedCam.mIsActive = false;
        selectedCam.mOutDated = true;
        mActiveGameCamera = false;
        // Unchecking a checked box defaults the editor to the main editor camera controller
        factory->setActiveCameraController(mEditorCamController);
    }
}
void InputSystem::setPitch(double pitch)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &cam{registry->get<GameCamera>(entityID)};
    cam.mPitch = pitch;
    cam.mOutDated = true;
}
