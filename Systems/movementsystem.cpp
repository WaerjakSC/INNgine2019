#include "movementsystem.h"

MovementSystem::MovementSystem(Pool<TransformComponent> *trans)
{
    transpool = trans;
}

void MovementSystem::update()
{
     for (int i = 0; i < transpool->size();  i++) {

         if(transpool->getComponents().at(i).mMatrixOutdated){
             transpool->getComponents().at(i).updateMatrix();
         }
         if(transpool->getComponents().at(i).parentID != -1){
            multiplyByParent(transpool->getEntityList().at(i),transpool->getComponents().at(i).parentID);
         }
     }


}

void MovementSystem::setPosition(int eID, gsl::Vector3D &position)
{
    transpool->get(eID)->mMatrixOutdated = true;

    transpool->get(eID)->mPosition = position;
}

void MovementSystem::setPosition(int eID, float xIn, float yIn, float zIn)
{
    transpool->get(eID)->mMatrixOutdated = true;
    transpool->get(eID)->mPosition.x = xIn;
    transpool->get(eID)->mPosition.y = yIn;
    transpool->get(eID)->mPosition.z = zIn;
}

void MovementSystem::setScale(int eID, gsl::Vector3D &scale)
{
    transpool->get(eID)->mMatrixOutdated = true;
   transpool->get(eID)-> mScale = scale;
}

void MovementSystem::setRotation(int eID, gsl::Vector3D &rotation)
{
    transpool->get(eID)->mMatrixOutdated = true;
    transpool->get(eID)->mRotation = rotation;
}

gsl::Matrix4x4 MovementSystem::multiplyByParent(int eID, int pID)
{
    if(transpool->get(pID)->parentID != -1)
        return multiplyByParent(eID, transpool->get(pID)->parentID);
    else{
       gsl::Matrix4x4 tempMatrix = *transpool->get(eID)->matrix() * *transpool->get(pID)->matrix();
       return tempMatrix;
    }
}








