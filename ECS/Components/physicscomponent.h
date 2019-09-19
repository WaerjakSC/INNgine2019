#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "component.h"

class PhysicsComponent : public Component {
public:
    PhysicsComponent();
    virtual void update(float dt);
};

#endif // PHYSICSCOMPONENT_H
