#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "pool.h"
#include <memory>
class MovementSystem {

public:
    MovementSystem(std::shared_ptr<Pool<TransformComponent>> trans);
    void update();

    void setPosition(int eID, gsl::Vector3D position);
    void setPosition(int eID, float xIn, float yIn, float zIn);

    void setPositionX(int eID, float xIn);
    void setPositionY(int eID, float yIn);
    void setPositionZ(int eID, float zIn);

    void setScale(int eID, gsl::Vector3D scale);
    void setRotation(int eID, gsl::Vector3D rotation);
    void moveX(int eID, float xIn);
    void moveY(int eID, float yIn);
    void moveZ(int eID, float zIn);

    gsl::Matrix4x4 multiplyByParent(int eID, int pID);
    void updateMatrix();
    void iterateChildren(int eID);

    gsl::Vector3D getPosition(int eID);

    gsl::Vector3D getRelativePosition(int eID);

    bool hasParent(int eID);

    void setRotationX(int eID, float xIn);
    void setRotationY(int eID, float yIn);
    void setRotationZ(int eID, float zIn);

    void setScaleX(int eID, float xIn);
    void setScaleY(int eID, float yIn);
    void setScaleZ(int eID, float zIn);

private:
    std::shared_ptr<Pool<TransformComponent>> mTransformPool;
};

#endif // MOVEMENTSYSTEM_H
