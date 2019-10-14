#include "movementsystem.h"
#include "billboard.h"
#include "registry.h"
MovementSystem::MovementSystem() {
    registry = Registry::instance();
    mTransforms = registry->registerComponent<Transform>();
}
void MovementSystem::init() {
    update();
}
void MovementSystem::update() {
    GLuint curEntity{0}; // which index of entities() is being iterated
    for (auto entityID : mTransforms->entities()) {
        auto &comp = mTransforms->data()[curEntity];
        if (comp.mMatrixOutdated) {
            if (comp.parentID != -1) {
                comp.mMatrix = multiplyByParent(entityID, comp.parentID);
                comp.mPosition = getPosition(entityID); // Position from the multiplied matrix should be the global position.
            } else {
                comp.mRelativePosition = gsl::Vector3D(0);
            }
            updateMatrix(curEntity, comp);
        }
        curEntity++;
    }
    for (auto billBoard : registry->billBoards()) {
        if (BillBoard *board = dynamic_cast<BillBoard *>(registry->getEntity(billBoard)))
            board->update();
    }
}
void MovementSystem::updateEntity(GLuint eID) {
    auto &comp = mTransforms->get(eID);
    if (comp.mMatrixOutdated) {
        if (comp.parentID != -1) {
            comp.mMatrix = multiplyByParent(eID, comp.parentID);
            comp.mPosition = getPosition(eID); // Position from the multiplied matrix should be the global position.
        } else {
            comp.mRelativePosition = gsl::Vector3D(0);
        }
        updateMatrix(eID, comp);
    }
}

/**
 * @brief Set child matrices to outdated if the parent moves so they also move.
 * @param eID
 */
void MovementSystem::iterateChildren(int eID) {
    if (eID != -1)
        for (auto entity : Registry::instance()->getComponent<Transform>(eID).mChildren) {
            Registry::instance()->getComponent<Transform>(entity).mMatrixOutdated = true;
        }
}

/**
 * @brief Get global position of object
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getPosition(int eID) {
    if (Registry::instance()->hasParent(eID)) {
        GLuint parentID = mTransforms->get(eID).parentID;
        return getPosition(parentID) + mTransforms->get(eID).mRelativePosition;
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
void MovementSystem::setPosition(GLuint eID, gsl::Vector3D position, bool signal) {
    mTransforms->get(eID).mMatrixOutdated = true;
    if (Registry::instance()->hasParent(eID)) {
        mTransforms->get(eID).mRelativePosition = position;
    } else {
        mTransforms->get(eID).mPosition = position;
    }
    iterateChildren(eID);
    if (signal)
        emit positionChanged(eID, position);
}
/**
 * @brief Sets relative position if parented to an object -- otherwise sets global position.
 * @param eID
 * @param position
 */
void MovementSystem::setPosition(int eID, float xIn, float yIn, float zIn, bool signal) {
    setPosition(eID, gsl::Vector3D{xIn, yIn, zIn}, signal);
}
//void MovementSystem::setRelativePosition(const vec3 &position) {
//}
void MovementSystem::setPositionX(int eID, float xIn, bool signal) {
    gsl::Vector3D newPos = getPosition(eID);
    newPos.x = xIn;

    setPosition(eID, newPos, signal);
}

void MovementSystem::setPositionY(int eID, float yIn, bool signal) {
    gsl::Vector3D newPos = getPosition(eID);
    newPos.y = yIn;

    setPosition(eID, newPos, signal);
}

void MovementSystem::setPositionZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newPos = getPosition(eID);
    newPos.z = zIn;

    setPosition(eID, newPos, signal);
}

void MovementSystem::moveX(int eID, float xIn) {
    gsl::Vector3D newPos = getPosition(eID);
    newPos.x += xIn;

    setPosition(eID, newPos);
}
void MovementSystem::moveY(int eID, float yIn) {
    gsl::Vector3D newPos = getPosition(eID);
    newPos.y += yIn;

    setPosition(eID, newPos);
}
void MovementSystem::moveZ(int eID, float zIn) {
    gsl::Vector3D newPos = getPosition(eID);
    newPos.z += zIn;

    setPosition(eID, newPos);
}

void MovementSystem::setRotation(GLuint eID, gsl::Vector3D rotation, bool signal) {
    mTransforms->get(eID).mMatrixOutdated = true;
    mTransforms->get(eID).mRotation = rotation;
    iterateChildren(eID);
    if (signal)
        emit rotationChanged(eID, rotation);
}
void MovementSystem::setRotationX(int eID, float xIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).mRotation;
    newRot.x = xIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationY(int eID, float yIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).mRotation;
    newRot.y = yIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).mRotation;
    newRot.z = zIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::rotateX(GLuint eID, float xIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).mRotation;
    newRot.x += xIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::rotateY(GLuint eID, float yIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).mRotation;
    newRot.y += yIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::rotateZ(GLuint eID, float zIn, bool signal) {
    gsl::Vector3D newRot = mTransforms->get(eID).mRotation;
    newRot.z += zIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setScale(int eID, gsl::Vector3D scale, bool signal) {
    mTransforms->get(eID).mMatrixOutdated = true;
    mTransforms->get(eID).mScale = scale;
    iterateChildren(eID);
    if (signal)
        emit scaleChanged(eID, scale);
}
void MovementSystem::setScaleX(int eID, float xIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).mScale;
    newScale.x = xIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleY(int eID, float yIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).mScale;
    newScale.y = yIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).mScale;
    newScale.z = zIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::scaleX(GLuint eID, float xIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).mScale;
    newScale.x += xIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::scaleY(GLuint eID, float yIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).mScale;
    newScale.y += yIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::scaleZ(GLuint eID, float zIn, bool signal) {
    gsl::Vector3D newScale = mTransforms->get(eID).mScale;
    newScale.z += zIn;
    setScale(eID, newScale, signal);
}
gsl::Matrix4x4 MovementSystem::multiplyByParent(GLuint eID, GLuint pID) {
    return mTransforms->get(pID).mMatrix * mTransforms->get(eID).mMatrix;
}

void MovementSystem::updateMatrix(GLuint eID, Transform &comp) {
    gsl::Vector3D position = getPosition(eID);
    gsl::Vector3D rotation = comp.mRotation;
    gsl::Vector3D scale = comp.mScale;
    //calculate matrix from position, scale, rotation
    comp.mMatrix.setToIdentity();
    comp.mMatrix.translate(position);
    comp.mMatrix.rotateX(rotation.x);
    comp.mMatrix.rotateY(rotation.y);
    comp.mMatrix.rotateZ(rotation.z);
    comp.mMatrix.scale(scale);
    comp.mMatrixOutdated = false;
}
