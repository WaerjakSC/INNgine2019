#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "pool.h"

class MovementSystem {
public:
    MovementSystem(Pool<TransformComponent> *trans);
    void update();

    void setPosition(int eID, gsl::Vector3D position);
    void setPosition(int eID, float xIn, float yIn, float zIn);
    void setScale(int eID, gsl::Vector3D scale);
    void setRotation(int eID, gsl::Vector3D rotation);

    gsl::Matrix4x4 multiplyByParent(int eID, int pID);
    void updateMatrix();
    void iterateChildren(int eID);
    void moveX(int eID, float xIn);
    void moveY(int eID, float yIn);
    void moveZ(int eID, float zIn);

    gsl::Vector3D getPosition(int eID);

    gsl::Vector3D getRelativePosition(int eID);

    bool hasParent(int eID);

private:
    Pool<TransformComponent> *transpool;
};

#endif // MOVEMENTSYSTEM_H
