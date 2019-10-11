#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "registry.h"


class CollisionSystem
{
public:
    CollisionSystem();

    bool IntersectAABB(int eID, int otherEID);
    bool IntersectOBB();
    Registry *reg = Registry::instance();


};

#endif // COLLISIONSYSTEM_H
