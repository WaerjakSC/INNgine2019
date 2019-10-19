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
void MovementSystem::update(float deltaTime) {
    Q_UNUSED(deltaTime) // remember to remove this once we implement deltaTime
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
    comp.translationMatrix.translate(comp.localPosition);

    comp.rotationMatrix.setToIdentity();
    comp.rotationMatrix.rotateX(comp.localRotation.x);
    comp.rotationMatrix.rotateY(comp.localRotation.y);
    comp.rotationMatrix.rotateZ(comp.localRotation.z);

    comp.scaleMatrix.setToIdentity();
    comp.scaleMatrix.scale(comp.localScale);
}
/**
 * @brief Get global position of object
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getAbsolutePosition(GLuint eID) {
    if (Registry::instance()->hasParent(eID)) {
        auto &trans = mTransforms->get(eID);
        trans.position = mTransforms->get(eID).modelMatrix.getPosition();
        return trans.position;
    }
    return getLocalPosition(eID);
}
/**
 * @brief Get relative position of object (null-vector if no parent)
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getLocalPosition(GLuint eID) {
    return mTransforms->get(eID).localPosition;
}
gsl::Vector3D MovementSystem::getAbsoluteRotation(GLuint eID) {
    if (Registry::instance()->hasParent(eID)) {
        auto &trans = mTransforms->get(eID);
        trans.rotation = std::get<2>(gsl::Matrix4x4::decomposed(trans.modelMatrix));
        return trans.rotation;
    }
    return getRelativeRotation(eID);
}
gsl::Vector3D MovementSystem::getRelativeRotation(GLuint eID) {
    return mTransforms->get(eID).localRotation;
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
    if (trans.parentID != -1) {
        const vec3 diff = position - getAbsolutePosition(eID);
        trans.localPosition += diff;
    } else {
        trans.localPosition = position;
    }
    trans.matrixOutdated = true;
    if (signal)
        emit positionChanged(eID, position, true);
}
/**
 * @brief Sets relative position -- equal to global pos if no parent
 * @param eID
 * @param position
 */
void MovementSystem::setLocalPosition(GLuint eID, gsl::Vector3D position, bool signal) {
    mTransforms->get(eID).localPosition = position;
    mTransforms->get(eID).matrixOutdated = true;
    if (signal)
        emit positionChanged(eID, position, false);
}
/**
 * @brief Sets relative position -- equal to global pos if no parent
 * @param eID
 * @param position
 */
void MovementSystem::setLocalPosition(int eID, float xIn, float yIn, float zIn, bool signal) {
    setLocalPosition(eID, gsl::Vector3D{xIn, yIn, zIn}, signal);
}

void MovementSystem::setLocalPositionX(int eID, float xIn, bool signal) {
    gsl::Vector3D newPos = getLocalPosition(eID);
    newPos.x = xIn;

    setLocalPosition(eID, newPos, signal);
}

void MovementSystem::setLocalPositionY(int eID, float yIn, bool signal) {
    gsl::Vector3D newPos = getLocalPosition(eID);
    newPos.y = yIn;

    setLocalPosition(eID, newPos, signal);
}

void MovementSystem::setLocalPositionZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newPos = getLocalPosition(eID);
    newPos.z = zIn;

    setLocalPosition(eID, newPos, signal);
}
void MovementSystem::setAbsolutePositionX(int eID, float xIn, bool signal) {
    gsl::Vector3D newPos = getAbsolutePosition(eID);
    newPos.x = xIn;

    setAbsolutePosition(eID, newPos, signal);
}

void MovementSystem::setAbsolutePositionY(int eID, float yIn, bool signal) {
    gsl::Vector3D newPos = getAbsolutePosition(eID);
    newPos.y = yIn;

    setAbsolutePosition(eID, newPos, signal);
}

void MovementSystem::setAbsolutePositionZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newPos = getAbsolutePosition(eID);
    newPos.z = zIn;

    setAbsolutePosition(eID, newPos, signal);
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
    vec3 position = getLocalPosition(eID);
    position += moveDelta;
    setLocalPosition(eID, position, signal);
}
void MovementSystem::setRotation(GLuint eID, gsl::Vector3D rotation, bool signal) {
    mTransforms->get(eID).matrixOutdated = true;
    mTransforms->get(eID).localRotation = rotation;
    if (signal)
        emit rotationChanged(eID, rotation);
}
void MovementSystem::setRotationX(int eID, float xIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).localRotation;
    newRot.x = xIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationY(int eID, float yIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).localRotation;
    newRot.y = yIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).localRotation;
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
    gsl::Vector3D rotation = mTransforms->get(eID).localRotation + rotDelta;
    setRotation(eID, rotation, signal);
}
void MovementSystem::setScale(int eID, gsl::Vector3D scale, bool signal) {
    auto &trans = mTransforms->get(eID);
    trans.localScale = scale;
    trans.matrixOutdated = true;
    if (signal)
        emit scaleChanged(eID, scale);
}
void MovementSystem::setScaleX(int eID, float xIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).localScale;
    newScale.x = xIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleY(int eID, float yIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).localScale;
    newScale.y = yIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).localScale;
    newScale.z = zIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::scale(GLuint eID, const vec3 &scaleDelta, bool signal) {
    gsl::Vector3D scale = mTransforms->get(eID).localScale + scaleDelta;
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
