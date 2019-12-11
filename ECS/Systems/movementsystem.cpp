#include "movementsystem.h"
#include "cameracontroller.h"
#include "registry.h"

MovementSystem::MovementSystem() : registry{Registry::instance()}
{
}
void MovementSystem::init()
{
    update();
}
void MovementSystem::update(DeltaTime dt)
{
    auto view{registry->view<Transform>()};
    for (auto entity : view) {
        auto view{registry->view<Transform>()};
        Transform &comp{view.get(entity)};
        if (comp.matrixOutdated) {
            updateColliders(entity);
        }
        updateModelMatrix(comp);
    }

    auto bulletview{registry->view<Transform, Bullet>()};
    for (auto entity : bulletview) {
        auto [trans, bullet]{bulletview.get<Transform, Bullet>(entity)};
        vec3 deltaVector = bullet.direction * dt * bullet.speed;
        move(entity, deltaVector);
    }

    auto aabbview{registry->view<Transform, AABB>()};
    for (auto entity : aabbview) {
        auto [trans, col]{aabbview.get<Transform, AABB>(entity)};
        if (col.transform.matrixOutdated) {
            updateTS(col);
            updateColliderTransformPrivate(col, trans);
        }
    }
    auto sphereview{registry->view<Transform, Sphere>()};
    for (auto entity : sphereview) {
        auto [trans, col]{sphereview.get<Transform, Sphere>(entity)};
        if (col.transform.matrixOutdated) {
            updateTS(col);
            updateColliderTransformPrivate(col, trans);
        }
    }
    auto billboardView{registry->view<BillBoard, Transform, Material>()};
    for (auto entity : billboardView) {
        auto [billboard, transform, mat]{billboardView.get<BillBoard, Transform, Material>(entity)};
        updateBillBoardTransformPrivate(billboard, transform, mat);
    }
}
void MovementSystem::updateModelMatrix(GLuint eID)
{
    auto view{registry->view<Transform>()};
    Transform &comp{view.get(eID)};
    updateModelMatrix(comp);
    updateColliders(eID);
}
void MovementSystem::updateModelMatrix(Transform &comp)
{
    if (comp.matrixOutdated) {
        //calculate matrix from position, scale, rotation
        updateTRS(comp);
        auto view{registry->view<Transform>()};
        if (comp.parentID != -1) {
            Transform &parent{view.get(comp.parentID)};
            gsl::Matrix4x4 parentMatrix{getTRMatrix(parent)};
            comp.modelMatrix = parentMatrix * comp.translationMatrix * comp.rotationMatrix * comp.scaleMatrix;
        }
        else {
            comp.position = comp.localPosition;
            comp.rotation = comp.localRotation;
            comp.modelMatrix = comp.translationMatrix * comp.rotationMatrix * comp.scaleMatrix;
        }
        comp.matrixOutdated = false;
        for (auto child : comp.children) {
            Transform &childComp{view.get(child)};
            childComp.matrixOutdated = true;
        }
    }
}
void MovementSystem::updateColliders(GLuint eID)
{
    if (registry->contains<AABB>(eID))
        registry->get<AABB>(eID).transform.matrixOutdated = true;
    else if (registry->contains<Sphere>(eID))
        registry->get<Sphere>(eID).transform.matrixOutdated = true;
}

void MovementSystem::updateColliderTransformPrivate(Collision &col, const Transform &trans)
{
    col.transform.modelMatrix = getTRMatrix(trans) * col.transform.translationMatrix * col.transform.scaleMatrix;
    col.transform.matrixOutdated = false;
}
void MovementSystem::updateBillBoardTransform(GLuint entity)
{
    auto view{registry->view<BillBoard, Transform, Material>()};
    auto [billboard, transform, mat]{view.get<BillBoard, Transform, Material>(entity)};
    updateBillBoardTransformPrivate(billboard, transform, mat);
}
void MovementSystem::updateBillBoardTransformPrivate(const BillBoard &billboard, Transform &transform, const Material &mat)
{
    // find direction between this and camera
    vec3 direction{};
    if (billboard.normalVersion) {
        vec3 camPosition{mat.shader->getCameraController()->cameraPosition()};
        //cancel height info so billboard is allways upright:
        if (billboard.constantYUp)
            camPosition.setY(transform.modelMatrix.getPosition().y);
        direction = camPosition - vec3(transform.modelMatrix.getPosition());
    }
    else {
        vec3 camDirection{mat.shader->getCameraController()->forward()};
        //cancel height info so billboard is allways upright:
        if (billboard.constantYUp)
            camDirection.setY(transform.modelMatrix.getPosition().y);
        direction = camDirection * -1;
    }
    direction.normalize();
    //set rotation to this direction
    transform.rotationMatrix.setRotationToVector(direction);
    transform.localRotation = std::get<2>(gsl::Matrix4x4::decomposed(transform.rotationMatrix));
    transform.matrixOutdated = true;
}

gsl::Matrix4x4 MovementSystem::getTRMatrix(const Transform &comp)
{
    if (comp.parentID != -1) {
        Transform &parent{registry->view<Transform>().get(comp.parentID)};
        gsl::Matrix4x4 parentMatrix{getTRMatrix(parent)};
        return parentMatrix * comp.translationMatrix * comp.rotationMatrix;
    }
    else
        return comp.translationMatrix * comp.rotationMatrix;
}
void MovementSystem::updateTRS(Transform &comp)
{
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

void MovementSystem::updateTS(AABB &comp)
{
    auto &trans{comp.transform};
    //calculate matrix from position, scale, rotation
    trans.translationMatrix.setToIdentity();
    trans.translationMatrix.translate(comp.origin);

    trans.scaleMatrix.setToIdentity();
    trans.scaleMatrix.scale(comp.size);
}
void MovementSystem::updateTS(Sphere &comp)
{
    auto &trans{comp.transform};
    //calculate matrix from position, scale, rotation
    trans.translationMatrix.setToIdentity();
    trans.translationMatrix.translate(comp.position);

    trans.scaleMatrix.setToIdentity();
    trans.scaleMatrix.scale(comp.radius);
}

gsl::Vector3D MovementSystem::getAbsolutePosition(GLuint eID)
{
    if (Registry::instance()->hasParent(eID)) {
        auto view{registry->view<Transform>()};
        auto &trans{view.get(eID)};
        trans.position = view.get(eID).modelMatrix.getPosition();
        return trans.position;
    }
    return getLocalPosition(eID);
}

gsl::Vector3D MovementSystem::getLocalPosition(GLuint eID)
{
    return registry->view<Transform>().get(eID).localPosition;
}
gsl::Vector3D MovementSystem::getAbsoluteRotation(GLuint eID)
{
    if (Registry::instance()->hasParent(eID)) {
        auto &trans{registry->view<Transform>().get(eID)};
        trans.rotation = std::get<2>(gsl::Matrix4x4::decomposed(trans.modelMatrix));
        return trans.rotation;
    }
    return getRelativeRotation(eID);
}
gsl::Vector3D MovementSystem::getRelativeRotation(GLuint eID)
{
    return registry->view<Transform>().get(eID).localRotation;
}
/**
 * @brief MovementSystem::setAbsolutePosition
 * @note don't use this function at the moment! doesn't work
 * @param eID
 * @param position
 * @param signal
 */
void MovementSystem::setAbsolutePosition(GLuint eID, vec3 position, bool signal)
{
    Transform &trans{registry->view<Transform>().get(eID)};
    if (trans.parentID != -1) {
        const vec3 diff{position - getAbsolutePosition(eID)};
        trans.localPosition += diff;
    }
    else {
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
void MovementSystem::setLocalPosition(GLuint eID, vec3 position, bool signal)
{
    auto view{registry->view<Transform>()};
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
void MovementSystem::setLocalPosition(int eID, float xIn, float yIn, float zIn, bool signal)
{
    setLocalPosition(eID, vec3{xIn, yIn, zIn}, signal);
}

void MovementSystem::setLocalPositionX(int eID, float xIn, bool signal)
{
    vec3 newPos{getLocalPosition(eID)};
    newPos.x = xIn;

    setLocalPosition(eID, newPos, signal);
}

void MovementSystem::setLocalPositionY(int eID, float yIn, bool signal)
{
    vec3 newPos{getLocalPosition(eID)};
    newPos.y = yIn;

    setLocalPosition(eID, newPos, signal);
}

void MovementSystem::setLocalPositionZ(int eID, float zIn, bool signal)
{
    vec3 newPos{getLocalPosition(eID)};
    newPos.z = zIn;

    setLocalPosition(eID, newPos, signal);
}
void MovementSystem::setAbsolutePositionX(int eID, float xIn, bool signal)
{
    vec3 newPos{getAbsolutePosition(eID)};
    newPos.x = xIn;

    setAbsolutePosition(eID, newPos, signal);
}

void MovementSystem::setAbsolutePositionY(int eID, float yIn, bool signal)
{
    vec3 newPos{getAbsolutePosition(eID)};
    newPos.y = yIn;

    setAbsolutePosition(eID, newPos, signal);
}

void MovementSystem::setAbsolutePositionZ(int eID, float zIn, bool signal)
{
    vec3 newPos{getAbsolutePosition(eID)};
    newPos.z = zIn;

    setAbsolutePosition(eID, newPos, signal);
}
void MovementSystem::moveX(GLuint eID, float xIn, bool signal)
{
    move(eID, vec3{xIn, 0, 0}, signal);
}
void MovementSystem::moveY(GLuint eID, float yIn, bool signal)
{
    move(eID, vec3{0, yIn, 0}, signal);
}
void MovementSystem::moveZ(GLuint eID, float zIn, bool signal)
{
    move(eID, vec3{0, 0, zIn}, signal);
}
void MovementSystem::move(GLuint eID, const vec3 &moveDelta, bool signal)
{
    vec3 position{getLocalPosition(eID)};
    position += moveDelta;
    setLocalPosition(eID, position, signal);
}
void MovementSystem::setRotation(GLuint eID, vec3 rotation, bool signal)
{
    auto view{registry->view<Transform>()};
    view.get(eID).matrixOutdated = true;
    view.get(eID).localRotation = rotation;
    if (signal)
        emit rotationChanged(eID, rotation);
}
void MovementSystem::setRotationX(int eID, float xIn, bool signal)
{
    vec3 newRot{registry->view<Transform>().get(eID).localRotation};
    newRot.x = xIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationY(int eID, float yIn, bool signal)
{
    vec3 newRot{registry->view<Transform>().get(eID).localRotation};
    newRot.y = yIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::setRotationZ(int eID, float zIn, bool signal)
{
    vec3 newRot{registry->view<Transform>().get(eID).localRotation};
    newRot.z = zIn;
    setRotation(eID, newRot, signal);
}
void MovementSystem::rotateX(GLuint eID, float xIn, bool signal)
{
    rotate(eID, vec3{xIn, 0, 0}, signal);
}
void MovementSystem::rotateY(GLuint eID, float yIn, bool signal)
{
    rotate(eID, vec3{0, yIn, 0}, signal);
}
void MovementSystem::rotateZ(GLuint eID, float zIn, bool signal)
{
    rotate(eID, vec3{0, 0, zIn}, signal);
}
void MovementSystem::rotate(GLuint eID, const vec3 &rotDelta, bool signal)
{
    vec3 rotation{registry->view<Transform>().get(eID).localRotation + rotDelta};
    setRotation(eID, rotation, signal);
}
void MovementSystem::setScale(GLuint eID, vec3 scale, bool signal)
{
    auto &trans{registry->view<Transform>().get(eID)};
    trans.localScale = scale;
    trans.matrixOutdated = true;
    if (signal)
        emit scaleChanged(eID, scale);
}
void MovementSystem::setScaleX(int eID, float xIn, bool signal)
{
    vec3 newScale{registry->view<Transform>().get(eID).localScale};
    newScale.x = xIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleY(int eID, float yIn, bool signal)
{
    vec3 newScale{registry->view<Transform>().get(eID).localScale};
    newScale.y = yIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::setScaleZ(int eID, float zIn, bool signal)
{
    vec3 newScale{registry->view<Transform>().get(eID).localScale};
    newScale.z = zIn;
    setScale(eID, newScale, signal);
}
void MovementSystem::scale(GLuint eID, const vec3 &scaleDelta, bool signal)
{
    vec3 scale{registry->view<Transform>().get(eID).localScale + scaleDelta};
    setScale(eID, scale, signal);
}
void MovementSystem::scaleX(GLuint eID, float xIn, bool signal)
{
    scale(eID, vec3{xIn, 0, 0}, signal);
}
void MovementSystem::scaleY(GLuint eID, float yIn, bool signal)
{
    scale(eID, vec3{0, yIn, 0}, signal);
}
void MovementSystem::scaleZ(GLuint eID, float zIn, bool signal)
{
    scale(eID, vec3{0, 0, zIn}, signal);
}
