#include "transformcomponent.h"

TransformComponent::TransformComponent() {
    mType = CType::Transform;
}

void TransformComponent::update(float dt) {
}

gsl::Matrix4x4 TransformComponent::matrix() const {
    return mMatrix;
}
