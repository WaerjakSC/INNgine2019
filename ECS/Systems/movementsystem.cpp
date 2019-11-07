#include "movementsystem.h"
#include "billboard.h"
#include "pool.h"
#include "registry.h"
MovementSystem::MovementSystem() {
    registry = Registry::instance();
}
void MovementSystem::init() {
    update();
}
void MovementSystem::update(float deltaTime) {
    Q_UNUSED(deltaTime) // remember to remove this once we implement deltaTime
    auto view = registry->view<Transform>();
    for (auto entity : view) {
        updateModelMatrix(entity);
    }
    auto aabbview = registry->view<Transform, AABB>();
    for (auto entity : aabbview) {
        updateCollider(entity);
    }
    for (auto billBoard : registry->billBoards()) {
        if (Ref<BillBoard> board = std::dynamic_pointer_cast<BillBoard>(registry->getEntity(billBoard)))
            board->update();
    }
}

void MovementSystem::updateCollider(GLuint entity) {
    auto view = registry->view<Transform, AABB>();
    auto [trans, col] = view.get<Transform, AABB>(entity);
    if (col.transform.matrixOutdated) {
        updateTS(col);
        col.transform.modelMatrix = getTRMatrix(trans) * col.transform.translationMatrix * col.transform.scaleMatrix;
        col.transform.matrixOutdated = false;
    }
}
/**
 * @brief Force update of matrix to an entity
 * @param eID
 */
void MovementSystem::updateEntity(GLuint eID) {
    auto &comp = registry->view<Transform>().get(eID);
    comp.matrixOutdated = true;
    updateModelMatrix(eID);
    for (auto child : comp.children)
        updateEntity(child);
    if (registry->contains<AABB>(eID))
        updateCollider(eID);
}

void MovementSystem::updateModelMatrix(GLuint eID) {
    auto view = registry->view<Transform>();
    Transform &comp = view.get(eID);
    if (comp.matrixOutdated) {
        //calculate matrix from position, scale, rotation
        updateTRS(comp);
        gsl::Matrix4x4 parentMatrix;
        auto view = registry->view<Transform>();
        if (comp.parentID != -1) {
            Transform &parent = view.get(comp.parentID);
            parentMatrix = getTRMatrix(parent);
        } else {
            parentMatrix.setToIdentity();
            comp.position = comp.localPosition;
            comp.rotation = comp.localRotation;
        }
        comp.modelMatrix = parentMatrix * comp.translationMatrix * comp.rotationMatrix * comp.scaleMatrix;
        comp.matrixOutdated = false;
        for (auto child : comp.children) {
            Transform &childComp = view.get(child);
            childComp.matrixOutdated = true;
        }
        if (registry->contains<AABB>(eID))
            registry->getComponent<AABB>(eID).transform.matrixOutdated = true;
    }
}
gsl::Matrix4x4 MovementSystem::getTRMatrix(Transform &comp) {
    gsl::Matrix4x4 parentMatrix;
    if (comp.parentID != -1) {
        Transform &parent = registry->view<Transform>().get(comp.parentID);
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
void MovementSystem::updateTS(AABB &comp) {
    auto &trans = comp.transform;
    //calculate matrix from position, scale, rotation
    trans.translationMatrix.setToIdentity();
    trans.translationMatrix.translate(comp.origin);

    trans.scaleMatrix.setToIdentity();
    trans.scaleMatrix.scale(comp.size); // This might need to be half of size
}
/**
 * @brief Get global position of object
 * @param eID
 * @return
 */
gsl::Vector3D MovementSystem::getAbsolutePosition(GLuint eID) {
    if (Registry::instance()->hasParent(eID)) {
        auto view = registry->view<Transform>();
        auto &trans = view.get(eID);
        trans.position = view.get(eID).modelMatrix.getPosition();
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
    return registry->view<Transform>().get(eID).localPosition;
}
gsl::Vector3D MovementSystem::getAbsoluteRotation(GLuint eID) {
    if (Registry::instance()->hasParent(eID)) {
        auto &trans = registry->view<Transform>().get(eID);
        trans.rotation = std::get<2>(gsl::Matrix4x4::decomposed(trans.modelMatrix));
        return trans.rotation;
    }
    return getRelativeRotation(eID);
}
gsl::Vector3D MovementSystem::getRelativeRotation(GLuint eID) {
    return registry->view<Transform>().get(eID).localRotation;
}
/**
 * @brief MovementSystem::setAbsolutePosition
 * @note don't use this function at the moment! doesn't work
 * @param eID
 * @param position
 * @param signal
 */
void MovementSystem::setAbsolutePosition(GLuint eID, gsl::Vector3D position, bool signal) {
    Transform &trans = registry->view<Transform>().get(eID);
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
    auto view = registry->view<Transform>();
    view.get(eID).localPosition = position;
    view.get(eID).matrixOutdated = true;
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
    auto view = registry->view<Transform>();
    view.get(eID).matrixOutdated = true;
    view.get(eID).localRotation = rotation;
    if (signal)
        emit rotationChanged(eID, rotation);
}
void MovementSystem::setRotationX(int eID, float xIn, bool signal) {
    gsl::Vector3D newRot = registry->view<Transform>().get(eID).localRotation;
    newRot.x = xIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationY(int eID, float yIn, bool signal) {
    gsl::Vector3D newRot = registry->view<Transform>().get(eID).localRotation;
    newRot.y = yIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newRot = registry->view<Transform>().get(eID).localRotation;
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
    gsl::Vector3D rotation = registry->view<Transform>().get(eID).localRotation + rotDelta;
    setRotation(eID, rotation, signal);
}
void MovementSystem::setScale(int eID, gsl::Vector3D scale, bool signal) {
    auto &trans = registry->view<Transform>().get(eID);
    trans.localScale = scale;
    trans.matrixOutdated = true;
    if (signal)
        emit scaleChanged(eID, scale);
}
void MovementSystem::setScaleX(int eID, float xIn, bool signal) {
    gsl::Vector3D newScale = registry->view<Transform>().get(eID).localScale;
    newScale.x = xIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleY(int eID, float yIn, bool signal) {
    gsl::Vector3D newScale = registry->view<Transform>().get(eID).localScale;
    newScale.y = yIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleZ(int eID, float zIn, bool signal) {
    gsl::Vector3D newScale = registry->view<Transform>().get(eID).localScale;
    newScale.z = zIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::scale(GLuint eID, const vec3 &scaleDelta, bool signal) {
    gsl::Vector3D scale = registry->view<Transform>().get(eID).localScale + scaleDelta;
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
