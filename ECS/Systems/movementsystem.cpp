#include "movementsystem.h"
#include "billboard.h"
#include "pool.h"
#include "registry.h"
MovementSystem::MovementSystem() {
    registry = Registry::instance();
    mTransforms = registry->registerComponent<Transform>();
}
void MovementSystem::init() {
    update();
}
void MovementSystem::update() {
    for (size_t curEntity{0}; curEntity < mTransforms->entities().size(); curEntity++) {
        auto &comp = mTransforms->data()[curEntity];
        updateModelMatrix(comp);
    }
    for (auto billBoard : registry->billBoards()) {
        if (BillBoard *board = dynamic_cast<BillBoard *>(registry->getEntity(billBoard)))
            board->update();
    }
}
/**
 * @brief Force update of matrix to an entity
 * @param eID
 */
void MovementSystem::updateEntity(GLuint eID) {
    auto &comp = mTransforms->get(eID);
    comp.matrixOutdated = true;
    updateModelMatrix(comp);
    for (auto child : comp.children)
        updateEntity(child);
}

void MovementSystem::updateModelMatrix(Transform &comp) {
    if (comp.matrixOutdated) {
        //calculate matrix from position, scale, rotation
        updateTRS(comp);
        gsl::Matrix4x4 parentMatrix;
        if (comp.parentID != -1) {
            Transform &parent = mTransforms->get(comp.parentID);
            parentMatrix = getTRMatrix(parent);
        } else
            parentMatrix.setToIdentity();
        comp.modelMatrix = parentMatrix * comp.translationMatrix * comp.rotationMatrix * comp.scaleMatrix;
        comp.matrixOutdated = false;
        for (auto child : comp.children) {
            Transform &childComp = mTransforms->get(child);
            childComp.matrixOutdated = true;
        }
    }
}
gsl::Matrix4x4 MovementSystem::getTRMatrix(Transform &comp) {
    gsl::Matrix4x4 parentMatrix;
    if (comp.parentID != -1) {
        Transform &parent = mTransforms->get(comp.parentID);
        parentMatrix = getTRMatrix(parent);
    } else
        parentMatrix.setToIdentity();
    return parentMatrix * comp.translationMatrix * comp.rotationMatrix;
}
void MovementSystem::updateTRS(Transform &comp) {
    //calculate matrix from position, scale, rotation
    comp.translationMatrix.setToIdentity();
    comp.translationMatrix.translate(comp.position);

    comp.rotationMatrix.setToIdentity();
    comp.rotationMatrix.rotateX(comp.rotation.x);
    comp.rotationMatrix.rotateY(comp.rotation.y);
    comp.rotationMatrix.rotateZ(comp.rotation.z);

    comp.scaleMatrix.setToIdentity();
    comp.scaleMatrix.scale(comp.scale);
}
/**
 * @brief Get global position of object
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getAbsolutePosition(int eID) {
    if (Registry::instance()->hasParent(eID)) {
        GLuint parentID = mTransforms->get(eID).parentID;
        return getAbsolutePosition(parentID) + getRelativePosition(eID);
    }
    return getRelativePosition(eID);
}
/**
 * @brief Get relative position of object (null-vector if no parent)
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getRelativePosition(int eID) {
    return mTransforms->get(eID).position;
}
/**
 * @brief MovementSystem::setAbsolutePosition
 * @note don't use this function at the moment! doesn't work
 * @param eID
 * @param position
 * @param signal
 */
void MovementSystem::setAbsolutePosition(GLuint eID, gsl::Vector3D position, bool signal) {
    Transform &trans = mTransforms->get(eID);
    trans.matrixOutdated = true;
    if (trans.parentID != -1) {
        vec3 desiredPos = -getAbsolutePosition(trans.parentID);
        mTransforms->get(eID).position = std::get<0>(gsl::Matrix4x4::decomposed((trans.modelMatrix)));
        qDebug() << trans.position;
    }
    if (signal)
        emit positionChanged(eID, position);
}
/**
 * @brief Sets relative position -- equal to global pos if no parent
 * @param eID
 * @param position
 */
void MovementSystem::setPosition(GLuint eID, gsl::Vector3D position, bool signal) {
    mTransforms->get(eID).matrixOutdated = true;
    mTransforms->get(eID).position = position;
    if (signal)
        emit positionChanged(eID, position);
}
/**
 * @brief Sets relative position -- equal to global pos if no parent
 * @param eID
 * @param position
 */
void MovementSystem::setPosition(int eID, float xIn, float yIn, float zIn, bool signal) {
    setPosition(eID, gsl::Vector3D{xIn, yIn, zIn}, signal);
}

void MovementSystem::setPositionX(int eID, float xIn, bool signal) {
    gsl::Vector3D newPos = getRelativePosition(eID);
    newPos.x = xIn;

    setPosition(eID, newPos, signal);
}

void MovementSystem::setPositionY(int eID, float yIn, bool signal) {
    gsl::Vector3D newPos = getRelativePosition(eID);
    newPos.y = yIn;

    setPosition(eID, newPos, signal);
}

void MovementSystem::setPositionZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newPos = getRelativePosition(eID);
    newPos.z = zIn;

    setPosition(eID, newPos, signal);
}

void MovementSystem::moveX(GLuint eID, float xIn, bool signal) {
    move(eID, vec3(xIn, 0, 0), signal);
}
void MovementSystem::moveY(GLuint eID, float yIn, bool signal) {
    move(eID, vec3(0, yIn, 0), signal);
}
void MovementSystem::moveZ(GLuint eID, float zIn, bool signal) {
    move(eID, vec3(0, 0, zIn), signal);
}
void MovementSystem::move(GLuint eID, const vec3 &moveDelta, bool signal) {
    vec3 position = getRelativePosition(eID);
    position += moveDelta;
    setPosition(eID, position, signal);
}
void MovementSystem::setRotation(GLuint eID, gsl::Vector3D rotation, bool signal) {
    mTransforms->get(eID).matrixOutdated = true;
    mTransforms->get(eID).rotation = rotation;
    if (signal)
        emit rotationChanged(eID, rotation);
}
void MovementSystem::setRotationX(int eID, float xIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).rotation;
    newRot.x = xIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationY(int eID, float yIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).rotation;
    newRot.y = yIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).rotation;
    newRot.z = zIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::rotateX(GLuint eID, float xIn, bool signal) {
    rotate(eID, vec3(xIn, 0, 0), signal);
}
void MovementSystem::rotateY(GLuint eID, float yIn, bool signal) {
    rotate(eID, vec3(0, yIn, 0), signal);
}
void MovementSystem::rotateZ(GLuint eID, float zIn, bool signal) {
    rotate(eID, vec3(0, 0, zIn), signal);
}
void MovementSystem::rotate(GLuint eID, const vec3 &rotDelta, bool signal) {
    gsl::Vector3D rotation = mTransforms->get(eID).rotation + rotDelta;
    setRotation(eID, rotation, signal);
}
void MovementSystem::setScale(int eID, gsl::Vector3D scale, bool signal) {
    mTransforms->get(eID).matrixOutdated = true;
    mTransforms->get(eID).scale = scale;
    if (signal)
        emit scaleChanged(eID, scale);
}
void MovementSystem::setScaleX(int eID, float xIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).scale;
    newScale.x = xIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleY(int eID, float yIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).scale;
    newScale.y = yIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).scale;
    newScale.z = zIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::scale(GLuint eID, const vec3 &scaleDelta, bool signal) {
    gsl::Vector3D scale = mTransforms->get(eID).scale + scaleDelta;
    setScale(eID, scale, signal);
}
void MovementSystem::scaleX(GLuint eID, float xIn, bool signal) {
    scale(eID, vec3(xIn, 0, 0), signal);
}
void MovementSystem::scaleY(GLuint eID, float yIn, bool signal) {
    scale(eID, vec3(0, yIn, 0), signal);
}
void MovementSystem::scaleZ(GLuint eID, float zIn, bool signal) {
    scale(eID, vec3(0, 0, zIn), signal);
}
