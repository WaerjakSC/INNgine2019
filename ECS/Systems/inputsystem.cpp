#include "inputsystem.h"
#include "camera.h"
#include "registry.h"
#include "resourcemanager.h"
InputSystem::InputSystem(Camera *currentCam) : registry(Registry::instance()), mCurrentCamera(currentCam) {
}

void InputSystem::update(float deltaTime) {
    handleGlobalInput();
}
void InputSystem::handleGlobalInput(QKeyEvent *event) {
    //Camera
    mCurrentCamera->setSpeed(0.f); //cancel last frame movement
    if (event->key() == Qt::Key_F) {
        emit snapSignal();
    }
    if (event->modifiers() == Qt::Key_Control && event->key() == Qt::Key_S) {
        factory->save();
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
