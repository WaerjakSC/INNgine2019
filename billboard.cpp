#include "billboard.h"
#include "camera.h"
#include "cameracontroller.h"
#include "registry.h"
#include "resourcemanager.h"
BillBoard::BillBoard(GLuint eID, const QString &name) : Entity(eID, name) {
    registry = Registry::instance();
    factory = ResourceManager::instance();
    setConstantYUp(false);
}

void BillBoard::update() {
    Transform &transform = registry->get<Transform>(id());
    Material &mat = registry->get<Material>(id());
    // find direction between this and camera
    vec3 direction{};
    if (mNormalVersion) {
        vec3 camPosition = mat.mShader->getCameraController()->cameraPosition();
        //cancel heigth info so billboard is allways upright:
        if (mConstantYUp)
            camPosition.setY(transform.modelMatrix.getPosition().y);
        direction = camPosition - vec3(transform.modelMatrix.getPosition());
    } else {
        vec3 camDirection = mat.mShader->getCameraController()->forward();
        //cancel heigth info so billboard is allways upright:
        if (mConstantYUp)
            camDirection.setY(transform.modelMatrix.getPosition().y);
        direction = camDirection * -1;
    }

    direction.normalize();
    //set rotation to this direction
    transform.rotationMatrix.setRotationToVector(direction);
    transform.localRotation = std::get<2>(gsl::Matrix4x4::decomposed(transform.rotationMatrix));
    transform.matrixOutdated = true;
}

//vec3 BillBoard::getNormal(gsl::Matrix4x4 mMatrix) {
//    gsl::Matrix3x3 tempMat = mMatrix.toMatrix3();

//    normal = tempMat * vec3{0.f, 0.f, -1.f};
//    normal.normalize();

//    return normal;
//}

void BillBoard::setConstantYUp(bool constantUp) {
    mConstantYUp = constantUp;
}
