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
