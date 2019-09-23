#ifndef COMPONENT_H
#define COMPONENT_H
#include "matrix4x4.h"
enum class CType {
    Transform = 0,
    Material = 1,
    Mesh = 2,
    Light = 3,
    Input = 4,
    Physics = 5,
    Sound = 6
}; // Consider using bitwise masks to let systems know what to ignore
typedef gsl::Vector3D vec3;
/**
 * @brief The Component class is the base class for all components.
 */
class Component {
public:
    Component() = default;
    virtual ~Component();

    virtual void init();
    virtual void update(float dt = 0.0f) = 0;

    CType type() const;

    //    bool operator<(Component const &comp);

protected:
    CType mType;
};
struct Transform : Component {
    Transform() {
        mType = CType::Transform;
        mMatrix.setToIdentity();
    }
    Transform(vec3 position, vec3 rotation = vec3(0), vec3 scale = vec3(1)) : Transform() {
        mPosition = position;
        mRotation = rotation;
        mScale = scale;
    }
    virtual void update(float dt = 0.0f) {}
    bool mMatrixOutdated{true};

    gsl::Matrix4x4 &matrix() { return mMatrix; };

    gsl::Vector3D mPosition{0};
    gsl::Vector3D mRelativePosition{0};
    gsl::Vector3D mRotation{0};
    gsl::Vector3D mScale{1};
    gsl::Matrix4x4 mMatrix;
    std::vector<int> mChildren;
    bool hasChildren{false};

    int parentID = -1;
};

#endif // COMPONENT_H
