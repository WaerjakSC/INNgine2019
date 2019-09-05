#ifndef COMPONENT_H
#define COMPONENT_H
#include "matrix4x4.h"

/**
 * @brief The Component class is the base class for all components.
 */
class Component {
public:
    Component();
    virtual ~Component();
    virtual void init() = 0;
    virtual void update(float dt) = 0;
};

#endif // COMPONENT_H
