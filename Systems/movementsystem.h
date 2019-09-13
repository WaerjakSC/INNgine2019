#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "pool.h"

class MovementSystem {
public:
    MovementSystem(Pool<TransformComponent> *trans);
    void update();

    void setPosition(int eID, gsl::Vector3D &position);
    void setPosition(int eID, float xIn, float yIn, float zIn);
    void setScale(int eID, gsl::Vector3D &scale);
    void setRotation(int eID, gsl::Vector3D &rotation);

    gsl::Matrix4x4 multiplyByParent(int eID, int pID);
    void updateMatrix();

private:
    Pool<TransformComponent> *transpool;
};

#endif // MOVEMENTSYSTEM_H
