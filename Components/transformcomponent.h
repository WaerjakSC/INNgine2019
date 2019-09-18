#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "component.h"
#include "vector3d.h"

class TransformComponent : public Component {
public:
    TransformComponent();
    virtual void update(float dt = 0.0f);
    bool mMatrixOutdated{true};

    gsl::Matrix4x4 &matrix();

    gsl::Vector3D mPosition{0};
    gsl::Vector3D mRelativePosition{0};
    gsl::Vector3D mRotation{0};
    gsl::Vector3D mScale{1};

    int parentID = -1;

    // Move to movement system later, transform component should only hold the values of position, rotation and scale
    // Thanks Ole! <3
    void setPosition(gsl::Vector3D &position);
    void setPosition(float xIn, float yIn, float zIn);
    void setScale(gsl::Vector3D &scale);
    void setRotation(gsl::Vector3D &rotation);

    gsl::Matrix4x4 multiplyByParent(gsl::Matrix4x4 matrixIn);
    void updateMatrix();

    std::vector<int> children() const;

    void addChild(const GLuint childID);
    void removeChild(const GLuint childID);

private:
    gsl::Matrix4x4 mMatrix;
    std::vector<int> mChildren;
    bool hasChildren{false};
};

#endif // TRANSFORMCOMPONENT_H
