#include "billboard.h"
#include "camera.h"

BillBoard::BillBoard(GLuint eID, std::string name) : GameObject(eID, name) {

    setConstantYUp(true);
}

BillBoard::~BillBoard() {
}

void BillBoard::update(/*MaterialComponent *material, TransformComponent *transform*/) {
    //find direction between this and camera
    //    gsl::Vector3D direction{};
    //    if (mNormalVersion) {
    //        gsl::Vector3D camPosition = material->getShader()->getCurrentCamera()->position();
    //        //cancel heigth info so billboard is allways upright:
    //        if (mConstantYUp)
    //            camPosition.setY(mMatrix.getPosition().y);
    //        direction = camPosition - gsl::Vector3D(mMatrix.getPosition());
    //    } else {
    //        gsl::Vector3D camDirection = material->getShader()->getCurrentCamera()->forward();
    //        //cancel heigth info so billboard is allways upright:
    //        if (mConstantYUp)
    //            camDirection.setY(mMatrix.getPosition().y);
    //        direction = camDirection * -1;
    //    }

    //    direction.normalize();
    //    //set rotation to this direction
    //    mMatrix.setRotationToVector(direction);

    //    glUseProgram(material->getShader()->getProgram());
    //    glBindVertexArray(mVAO);
    //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //    glBindVertexArray(0);
}

gsl::Vector3D BillBoard::getNormal(gsl::Matrix4x4 mMatrix) {
    gsl::Matrix3x3 tempMat = mMatrix.toMatrix3();

    normal = tempMat * gsl::Vector3D{0.f, 0.f, -1.f};
    normal.normalize();

    return normal;
}

void BillBoard::setConstantYUp(bool constantUp) {
    mConstantYUp = constantUp;
}
