#include "movementsystem.h"

MovementSystem::MovementSystem(Pool<TransformComponent> *trans) : transpool(trans) {
}
void MovementSystem::update() {
    for (size_t i = 0; i < transpool->size(); i++) {
        if (transpool->getComponents().at(i)->mMatrixOutdated) {
            transpool->getComponents().at(i)->updateMatrix();
            if (transpool->getComponents().at(i)->parentID != -1) {
                transpool->getComponents().at(i)->matrix() = multiplyByParent(transpool->getEntityList().at(i), transpool->getComponents().at(i)->parentID);
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
void MovementSystem::setPosition(int eID, gsl::Vector3D position) {
    transpool->get(eID)->mMatrixOutdated = true;
    transpool->get(eID)->mPosition = position;
    iterateChildren(eID);
}
void MovementSystem::moveX(int eID, float xIn) {
    transpool->get(eID)->mMatrixOutdated = true;
    transpool->get(eID)->mPosition.x += xIn;
    iterateChildren(eID);
}
void MovementSystem::moveY(int eID, float yIn) {
    transpool->get(eID)->mMatrixOutdated = true;
    transpool->get(eID)->mPosition.y += yIn;
    iterateChildren(eID);
}
void MovementSystem::moveZ(int eID, float zIn) {
    transpool->get(eID)->mMatrixOutdated = true;
    transpool->get(eID)->mPosition.z += zIn;
    iterateChildren(eID);
}

void MovementSystem::setPosition(int eID, float xIn, float yIn, float zIn) {
    transpool->get(eID)->mMatrixOutdated = true;
    transpool->get(eID)->mPosition.x = xIn;
    transpool->get(eID)->mPosition.y = yIn;
    transpool->get(eID)->mPosition.z = zIn;
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
