#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "registry.h"


class CollisionSystem
{
public:
    CollisionSystem();

  //  bool IntersectAABB(int eID, int otherEID);
    bool IntersectOBB();
    Registry *reg = Registry::instance();
    static bool IntersectAABBAABB(const std::pair<vec3, vec3>& a, const std::pair<vec3, vec3>b);


};

#endif // COLLISIONSYSTEM_H
