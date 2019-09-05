#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "gameobject.h"
#include "vector3d.h"
class TransformComponent;
class MaterialComponent;
class BillBoard : public GameObject {
public:
    BillBoard(GLuint eID, std::string name);
    ~BillBoard();

    virtual void update() override;

    gsl::Vector3D getNormal(gsl::Matrix4x4 mMatrix);

    void setConstantYUp(bool constantUp);

    void update(MaterialComponent *material, TransformComponent *transform);

private:
    gsl::Vector3D normal{0.f, 0.f, -1.f};

    bool mConstantYUp{true};
    bool mNormalVersion{false}; //flip between two ways to calculate forward direction
};

#endif // BILLBOARD_H
