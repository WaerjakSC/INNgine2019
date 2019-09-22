#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "component.h"
#include "vector3d.h"

class Transform : public Component {
public:
    Transform();
    virtual void update(float dt = 0.0f);
    bool mMatrixOutdated{true};

    gsl::Matrix4x4 &matrix();

    gsl::Vector3D mPosition{0};
    gsl::Vector3D mRelativePosition{0};
    gsl::Vector3D mRotation{0};
    gsl::Vector3D mScale{1};

    int parentID = -1;

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
