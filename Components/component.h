#ifndef COMPONENT_H
#define COMPONENT_H
#include "matrix4x4.h"

#include "matrix4x4.h"
enum CType {
    Transform = 0,
    Material = 1,
    Mesh = 2,
    Light = 3,
    Input = 4,
    Physics = 5,
    Sound = 6
}; // Consider using bitwise masks to let systems know what to ignore

/**
 * @brief The Component class is the base class for all components.
 */
class Component : public QObject {
public:
    Component();
    virtual ~Component();

    virtual void init();
    virtual void update(float dt = 0.0f) = 0;

    CType type() const;

    //    bool operator<(Component const &comp);

protected:
    CType mType;
};

#endif // COMPONENT_H
