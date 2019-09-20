#include "movementsystem.h"

MovementSystem::MovementSystem(std::shared_ptr<Pool<TransformComponent>> trans) : mTransformPool(trans) {
}

void MovementSystem::update() {
    for (size_t i = 0; i < mTransformPool->size(); i++) {
        TransformComponent *comp = mTransformPool->getComponents().at(i);
        if (comp->mMatrixOutdated) {
            comp->updateMatrix();
            if (comp->parentID != -1) {
                comp->matrix() = multiplyByParent(mTransformPool->getEntityList().at(i), comp->parentID);
                if (comp->mPosition != getPosition(mTransformPool->getEntityList().at(i))) {
                    comp->mPosition = getPosition(mTransformPool->getEntityList().at(i)); // Position from the multiplied matrix should be the global position.
                    comp->mMatrixOutdated = true;
                }
            } else {
                comp->mRelativePosition = gsl::Vector3D(0);
            }
        }
    }
}
/**
 * @brief Set child matrices to outdated if the parent moves so they also move.
 * @param eID
 */
void MovementSystem::iterateChildren(int eID) {
    for (auto entity : mTransformPool->get(eID)->children()) {
        mTransformPool->get(entity)->mMatrixOutdated = true;
    }
}
/**
 * @brief Get global position of object
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getPosition(int eID) {
    if (hasParent(eID)) {
        GLuint parentID = mTransformPool->get(eID)->parentID;
        return mTransformPool->get(parentID)->mPosition + mTransformPool->get(eID)->mRelativePosition;
    }
    return mTransformPool->get(eID)->mPosition;
}
/**
 * @brief Get relative position of object (null-vector if no parent)
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getRelativePosition(int eID) {
    return mTransformPool->get(eID)->mRelativePosition;
}
/**
 * @brief Sets relative position if parented to an object -- otherwise sets global position.
 * @param eID
 * @param position
 */
void MovementSystem::setPosition(int eID, gsl::Vector3D position) {
    mTransformPool->get(eID)->mMatrixOutdated = true;
    if (hasParent(eID)) {
        mTransformPool->get(eID)->mRelativePosition = position;
    } else {
        mTransformPool->get(eID)->mPosition = position;
    }
    iterateChildren(eID);
    emit positionChanged(position);
}
/**
 * @brief Sets relative position if parented to an object -- otherwise sets global position.
 * @param eID
 * @param position
 */
void MovementSystem::setPosition(int eID, float xIn, float yIn, float zIn) {
    setPosition(eID, gsl::Vector3D{xIn, yIn, zIn});
}
void MovementSystem::setPositionX(int eID, float xIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransformPool->get(eID)->mRelativePosition;
        newPos.x = xIn;
    } else {
        newPos = mTransformPool->get(eID)->mPosition;
        newPos.x = xIn;
    }
    setPosition(eID, newPos);
}

void MovementSystem::setPositionY(int eID, float yIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransformPool->get(eID)->mRelativePosition;
        newPos.y = yIn;
    } else {
        newPos = mTransformPool->get(eID)->mPosition;
        newPos.y = yIn;
    }
    setPosition(eID, newPos);
}

void MovementSystem::setPositionZ(int eID, float zIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransformPool->get(eID)->mRelativePosition;
        newPos.z = zIn;
    } else {
        newPos = mTransformPool->get(eID)->mPosition;
        newPos.z = zIn;
    }
    setPosition(eID, newPos);
}
bool MovementSystem::hasParent(int eID) {
    return mTransformPool->get(eID)->parentID != -1;
}
void MovementSystem::moveX(int eID, float xIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransformPool->get(eID)->mRelativePosition;
        newPos.x += xIn;
    } else {
        newPos = mTransformPool->get(eID)->mPosition;
        newPos.x += xIn;
    }
    setPosition(eID, newPos);
}
void MovementSystem::moveY(int eID, float yIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransformPool->get(eID)->mRelativePosition;
        newPos.y += yIn;
    } else {
        newPos = mTransformPool->get(eID)->mPosition;
        newPos.y += yIn;
    }
    setPosition(eID, newPos);
}
void MovementSystem::moveZ(int eID, float zIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransformPool->get(eID)->mRelativePosition;
        newPos.z += zIn;
    } else {
        newPos = mTransformPool->get(eID)->mPosition;
        newPos.z += zIn;
    }
    setPosition(eID, newPos);
}

void MovementSystem::setRotation(int eID, gsl::Vector3D rotation) {
    mTransformPool->get(eID)->mMatrixOutdated = true;
    mTransformPool->get(eID)->mRotation = rotation;
    iterateChildren(eID);
    emit rotationChanged(rotation);
}
void MovementSystem::setRotationX(int eID, float xIn) {
    gsl::Vector3D newRot = mTransformPool->get(eID)->mRotation;
    newRot.x = xIn;
    setRotation(eID, newRot);
}
void MovementSystem::setRotationY(int eID, float yIn) {
    gsl::Vector3D newRot = mTransformPool->get(eID)->mRotation;
    newRot.y = yIn;
    setRotation(eID, newRot);
}
void MovementSystem::setRotationZ(int eID, float zIn) {
    gsl::Vector3D newRot = mTransformPool->get(eID)->mRotation;
    newRot.z = zIn;
    setRotation(eID, newRot);
}
void MovementSystem::setScale(int eID, gsl::Vector3D scale) {
    mTransformPool->get(eID)->mMatrixOutdated = true;
    mTransformPool->get(eID)->mScale = scale;
    iterateChildren(eID);
    emit scaleChanged(scale);
}
void MovementSystem::setScaleX(int eID, float xIn) {
    gsl::Vector3D newScale = mTransformPool->get(eID)->mScale;
    newScale.x = xIn;
    setScale(eID, newScale);
}
void MovementSystem::setScaleY(int eID, float yIn) {
    gsl::Vector3D newScale = mTransformPool->get(eID)->mScale;
    newScale.y = yIn;
    setScale(eID, newScale);
}
void MovementSystem::setScaleZ(int eID, float zIn) {
    gsl::Vector3D newScale = mTransformPool->get(eID)->mScale;
    newScale.z = zIn;
    setScale(eID, newScale);
}
gsl::Matrix4x4 MovementSystem::multiplyByParent(int eID, int pID) {
    return mTransformPool->get(pID)->matrix() * mTransformPool->get(eID)->matrix();
}
