#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "component.h"

class Physics : public Component {
public:
    Physics();
    virtual void update(float dt);
};

#endif // PHYSICSCOMPONENT_H
