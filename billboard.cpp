#include "billboard.h"
#include "camera.h"

BillBoard::BillBoard(GLuint eID, std::string name) : GameObject(eID, name) {

    setConstantYUp(true);
}

void BillBoard::update(Transform *transform, Shader *shader) {
    // find direction between this and camera
    gsl::Vector3D direction{};
    if (mNormalVersion) {
        gsl::Vector3D camPosition = shader->getCurrentCamera()->position();
        //cancel heigth info so billboard is allways upright:
        if (mConstantYUp)
            camPosition.setY(transform->mMatrix.getPosition().y);
        direction = camPosition - gsl::Vector3D(transform->mMatrix.getPosition());
    } else {
        gsl::Vector3D camDirection = shader->getCurrentCamera()->forward();
        //cancel heigth info so billboard is allways upright:
        if (mConstantYUp)
            camDirection.setY(transform->mMatrix.getPosition().y);
        direction = camDirection * -1;
    }

    direction.normalize();
    //set rotation to this direction
    transform->mMatrix.setRotationToVector(direction);
}

//gsl::Vector3D BillBoard::getNormal(gsl::Matrix4x4 mMatrix) {
//    gsl::Matrix3x3 tempMat = mMatrix.toMatrix3();

//    normal = tempMat * gsl::Vector3D{0.f, 0.f, -1.f};
//    normal.normalize();

//    return normal;
//}

void BillBoard::setConstantYUp(bool constantUp) {
    mConstantYUp = constantUp;
}
