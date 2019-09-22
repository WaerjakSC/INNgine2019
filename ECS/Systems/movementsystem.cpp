#include "movementsystem.h"
#include "registry.h"
MovementSystem::MovementSystem() {
    mTransforms = Registry::instance()->registerComponent<Transform>();
}

void MovementSystem::update() {
    GLuint curEntity{0}; // which index of entities() is being iterated
    for (auto entityID : mTransforms->entities()) {
        auto &comp = mTransforms->data()[curEntity];
        if (comp.mMatrixOutdated) {
            comp.updateMatrix();
            if (comp.parentID != -1) {
                comp.matrix() = multiplyByParent(entityID, comp.parentID);
                if (comp.mPosition != getPosition(entityID)) { // This is probably dumb
                    comp.mPosition = getPosition(entityID);    // Position from the multiplied matrix should be the global position.
                    comp.mMatrixOutdated = true;
                }
            } else {
                comp.mRelativePosition = gsl::Vector3D(0);
            }
        }
        curEntity++;
    }
}
/**
 * @brief Set the parent of a gameobject (or rather its transform component).
 * Note: Currently no support for setting an item to be a child of a previously created item, due to how we're currently inserting into items into the view.
 * For now, make sure you create items in the order you want them to be parented, i.e. Parent first, then Children.
 * @param eID
 * @param parentID
 */
void MovementSystem::setParent(int eID, int newParentID) {
    int oldParentID = mTransforms->get(eID).parentID;
    if (oldParentID != -1) // Make sure to remove the child from its old parent if it had one
        mTransforms->get(oldParentID).removeChild(eID);
    mTransforms->get(eID).parentID = newParentID; // Set the new parent ID. Can be set to -1 if you want it to be independent again.
    if (newParentID != -1)
        mTransforms->get(newParentID).addChild(eID);
}
/**
 * @brief Set child matrices to outdated if the parent moves so they also move.
 * @param eID
 */
void MovementSystem::iterateChildren(int eID) {
    for (auto entity : mTransforms->get(eID).children()) {
        mTransforms->get(entity).mMatrixOutdated = true;
    }
}
/**
 * @brief Get global position of object
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getPosition(int eID) {
    if (hasParent(eID)) {
        GLuint parentID = mTransforms->get(eID).parentID;
        return mTransforms->get(parentID).mPosition + mTransforms->get(eID).mRelativePosition;
    }
    return mTransforms->get(eID).mPosition;
}
/**
 * @brief Get relative position of object (null-vector if no parent)
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getRelativePosition(int eID) {
    return mTransforms->get(eID).mRelativePosition;
}
/**
 * @brief Sets relative position if parented to an object -- otherwise sets global position.
 * @param eID
 * @param position
 */
void MovementSystem::setPosition(int eID, gsl::Vector3D position) {
    mTransforms->get(eID).mMatrixOutdated = true;
    if (hasParent(eID)) {
        mTransforms->get(eID).mRelativePosition = position;
    } else {
        mTransforms->get(eID).mPosition = position;
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
        newPos = mTransforms->get(eID).mRelativePosition;
        newPos.x = xIn;
    } else {
        newPos = mTransforms->get(eID).mPosition;
        newPos.x = xIn;
    }
    setPosition(eID, newPos);
}

void MovementSystem::setPositionY(int eID, float yIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransforms->get(eID).mRelativePosition;
        newPos.y = yIn;
    } else {
        newPos = mTransforms->get(eID).mPosition;
        newPos.y = yIn;
    }
    setPosition(eID, newPos);
}

void MovementSystem::setPositionZ(int eID, float zIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransforms->get(eID).mRelativePosition;
        newPos.z = zIn;
    } else {
        newPos = mTransforms->get(eID).mPosition;
        newPos.z = zIn;
    }
    setPosition(eID, newPos);
}
bool MovementSystem::hasParent(int eID) {
    return mTransforms->get(eID).parentID != -1;
}
void MovementSystem::moveX(int eID, float xIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransforms->get(eID).mRelativePosition;
        newPos.x += xIn;
    } else {
        newPos = mTransforms->get(eID).mPosition;
        newPos.x += xIn;
    }
    setPosition(eID, newPos);
}
void MovementSystem::moveY(int eID, float yIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransforms->get(eID).mRelativePosition;
        newPos.y += yIn;
    } else {
        newPos = mTransforms->get(eID).mPosition;
        newPos.y += yIn;
    }
    setPosition(eID, newPos);
}
void MovementSystem::moveZ(int eID, float zIn) {
    gsl::Vector3D newPos;
    if (hasParent(eID)) {
        newPos = mTransforms->get(eID).mRelativePosition;
        newPos.z += zIn;
    } else {
        newPos = mTransforms->get(eID).mPosition;
        newPos.z += zIn;
    }
    setPosition(eID, newPos);
}

void MovementSystem::setRotation(int eID, gsl::Vector3D rotation) {
    mTransforms->get(eID).mMatrixOutdated = true;
    mTransforms->get(eID).mRotation = rotation;
    iterateChildren(eID);
    emit rotationChanged(rotation);
}
void MovementSystem::setRotationX(int eID, float xIn) {
    gsl::Vector3D newRot = mTransforms->get(eID).mRotation;
    newRot.x = xIn;
    setRotation(eID, newRot);
}
void MovementSystem::setRotationY(int eID, float yIn) {
    gsl::Vector3D newRot = mTransforms->get(eID).mRotation;
    newRot.y = yIn;
    setRotation(eID, newRot);
}
void MovementSystem::setRotationZ(int eID, float zIn) {
    gsl::Vector3D newRot = mTransforms->get(eID).mRotation;
    newRot.z = zIn;
    setRotation(eID, newRot);
}
void MovementSystem::setScale(int eID, gsl::Vector3D scale) {
    mTransforms->get(eID).mMatrixOutdated = true;
    mTransforms->get(eID).mScale = scale;
    iterateChildren(eID);
    emit scaleChanged(scale);
}
void MovementSystem::setScaleX(int eID, float xIn) {
    gsl::Vector3D newScale = mTransforms->get(eID).mScale;
    newScale.x = xIn;
    setScale(eID, newScale);
}
void MovementSystem::setScaleY(int eID, float yIn) {
    gsl::Vector3D newScale = mTransforms->get(eID).mScale;
    newScale.y = yIn;
    setScale(eID, newScale);
}
void MovementSystem::setScaleZ(int eID, float zIn) {
    gsl::Vector3D newScale = mTransforms->get(eID).mScale;
    newScale.z = zIn;
    setScale(eID, newScale);
}
gsl::Matrix4x4 MovementSystem::multiplyByParent(int eID, int pID) {
    return mTransforms->get(pID).matrix() * mTransforms->get(eID).matrix();
}
