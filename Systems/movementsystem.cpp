#include "movementsystem.h"

MovementSystem::MovementSystem(Pool<TransformComponent> *trans) : transpool(trans) {
}
void MovementSystem::update() {
    for (size_t i = 0; i < transpool->size(); i++) {
        TransformComponent *comp = transpool->getComponents().at(i);
        if (comp->mMatrixOutdated) {
            comp->updateMatrix();
            if (comp->parentID != -1) {
                comp->matrix() = multiplyByParent(transpool->getEntityList().at(i), comp->parentID);
                if (comp->mPosition != getPosition(transpool->getEntityList().at(i))) {
                    comp->mPosition = getPosition(transpool->getEntityList().at(i)); // Position from the multiplied matrix should be the global position.
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
    for (auto entity : transpool->get(eID)->children()) {
        transpool->get(entity)->mMatrixOutdated = true;
    }
}
/**
 * @brief Get global position of object
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getPosition(int eID) {
    if (hasParent(eID)) {
        GLuint parentID = transpool->get(eID)->parentID;
        return transpool->get(parentID)->mPosition + transpool->get(eID)->mRelativePosition;
    }
    return transpool->get(eID)->mPosition;
}
/**
 * @brief Get relative position of object (null-vector if no parent)
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getRelativePosition(int eID) {
    return transpool->get(eID)->mRelativePosition;
}
/**
 * @brief Sets relative position if parented to an object -- otherwise sets global position.
 * @param eID
 * @param position
 */
void MovementSystem::setPosition(int eID, gsl::Vector3D position) {
    transpool->get(eID)->mMatrixOutdated = true;
    if (hasParent(eID)) {
        transpool->get(eID)->mRelativePosition = position;
    } else {
        transpool->get(eID)->mPosition = position;
    }
    iterateChildren(eID);
}
/**
 * @brief Sets relative position if parented to an object -- otherwise sets global position.
 * @param eID
 * @param position
 */
void MovementSystem::setPosition(int eID, float xIn, float yIn, float zIn) {
    transpool->get(eID)->mMatrixOutdated = true;
    if (hasParent(eID)) {
        transpool->get(eID)->mRelativePosition.x = xIn;
        transpool->get(eID)->mRelativePosition.y = yIn;
        transpool->get(eID)->mRelativePosition.z = zIn;
    } else {
        transpool->get(eID)->mPosition.x = xIn;
        transpool->get(eID)->mPosition.y = yIn;
        transpool->get(eID)->mPosition.z = zIn;
    }
    iterateChildren(eID);
}
bool MovementSystem::hasParent(int eID) {
    return transpool->get(eID)->parentID != -1;
}
void MovementSystem::moveX(int eID, float xIn) {
    transpool->get(eID)->mMatrixOutdated = true;
    if (hasParent(eID))
        transpool->get(eID)->mRelativePosition.x += xIn;
    else
        transpool->get(eID)->mPosition.x += xIn;
    iterateChildren(eID);
}
void MovementSystem::moveY(int eID, float yIn) {
    transpool->get(eID)->mMatrixOutdated = true;
    if (hasParent(eID))
        transpool->get(eID)->mRelativePosition.y += yIn;
    else
        transpool->get(eID)->mPosition.y += yIn;
    iterateChildren(eID);
}
void MovementSystem::moveZ(int eID, float zIn) {
    transpool->get(eID)->mMatrixOutdated = true;
    if (hasParent(eID))
        transpool->get(eID)->mRelativePosition.z += zIn;
    else
        transpool->get(eID)->mPosition.z += zIn;
    iterateChildren(eID);
}

void MovementSystem::setScale(int eID, gsl::Vector3D scale) {
    transpool->get(eID)->mMatrixOutdated = true;
    transpool->get(eID)->mScale = scale;
    iterateChildren(eID);
}

void MovementSystem::setRotation(int eID, gsl::Vector3D rotation) {
    transpool->get(eID)->mMatrixOutdated = true;
    transpool->get(eID)->mRotation = rotation;
    iterateChildren(eID);
}

gsl::Matrix4x4 MovementSystem::multiplyByParent(int eID, int pID) {
    return transpool->get(pID)->matrix() * transpool->get(eID)->matrix();
}
