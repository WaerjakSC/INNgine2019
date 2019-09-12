#include "transformcomponent.h"

TransformComponent::TransformComponent() {
    mType = CType::Transform;
    mMatrix.setToIdentity();
}

void TransformComponent::update(float dt) {
}

gsl::Matrix4x4 *TransformComponent::matrix() {
    if (mMatrixOutdated) {
        updateMatrix();
    }
    return &mMatrix;
}

void TransformComponent::setPosition(gsl::Vector3D &position) {
    mMatrixOutdated = true;
    mPosition = position;
}

void TransformComponent::setPosition(float xIn, float yIn, float zIn) {
    mMatrixOutdated = true;
    mPosition.x = xIn;
    mPosition.y = yIn;
    mPosition.z = zIn;
}

void TransformComponent::setScale(gsl::Vector3D &scale) {
    mMatrixOutdated = true;
    mScale = scale;
}

void TransformComponent::setRotation(gsl::Vector3D &rotation) {
    mMatrixOutdated = true;
    mRotation = rotation;
}

gsl::Matrix4x4 TransformComponent::multiplyByParent(gsl::Matrix4x4 matrixIn) {
}

void TransformComponent::updateMatrix() {
    if (mMatrixOutdated) {
        //calculate matrix from position, scale, rotation
        mMatrix.setToIdentity();
        mMatrix.translate(mPosition);
        mMatrix.rotateX(mRotation.x);
        mMatrix.rotateY(mRotation.y);
        mMatrix.rotateZ(mRotation.z);
        mMatrix.scale(mScale);
        mMatrixOutdated = false;
    }
}
