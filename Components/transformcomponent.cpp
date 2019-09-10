#include "transformcomponent.h"

TransformComponent::TransformComponent() {
    mType = CType::Transform;
    mMatrix.setToIdentity();
}

void TransformComponent::update(float dt) {
}

gsl::Matrix4x4 *TransformComponent::matrix() {
    return &mMatrix;
}
