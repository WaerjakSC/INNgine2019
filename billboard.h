#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "entity.h"
class Transform;
class ResourceManager;
class Registry;
class BillBoard : public Entity {

public:
    BillBoard(GLuint eID, const QString &name);

    //    gsl::Vector3D getNormal(gsl::Matrix4x4 mMatrix);

    void setConstantYUp(bool constantUp);

    void update();

private:
    //    gsl::Vector3D normal{0.f, 0.f, -1.f};

    bool mConstantYUp{true};
    bool mNormalVersion{false}; //flip between two ways to calculate forward direction
    Registry *registry;
    ResourceManager *factory;
};

#endif // BILLBOARD_H
