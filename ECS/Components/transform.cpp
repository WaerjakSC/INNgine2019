#include "transform.h"

Transform::Transform() {
    mType = CType::Transform;
    mMatrix.setToIdentity();
}

Transform::Transform(vec3 position, vec3 rotation, vec3 scale) : Transform() {
    mPosition = position;
    mRotation = rotation;
    mScale = scale;
}

void Transform::update(float dt) {
}

gsl::Matrix4x4 &Transform::matrix() {
    if (mMatrixOutdated) {
        updateMatrix();
    }
    return mMatrix;
}

void Transform::updateMatrix() {
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

std::vector<int> Transform::children() const {
    return mChildren;
}

void Transform::addChild(const GLuint childID) {
    mChildren.emplace_back(childID);
    hasChildren = true;
}

void Transform::removeChild(const GLuint childID) {
    for (auto child : mChildren) {
        if (child == (int)childID) {
            std::swap(child, mChildren.back());
            mChildren.pop_back();
        }
    }
    if (mChildren.empty())
        hasChildren = false;
}
