#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "registry.h"
#include "components.h"

class CollisionSystem
{
public:
    CollisionSystem();

  //  bool IntersectAABB(int eID, int otherEID);
    bool IntersectOBB();
    Registry *reg = Registry::instance();
    static bool IntersectAABBAABB(const std::pair<vec3, vec3>& a, const std::pair<vec3, vec3>b);

    vec3 getMin(const Collision::AABB& aabb);
    vec3 getMax(const Collision::AABB& aabb);


    bool AABBAABB(const Collision::AABB& AABB1, const Collision::AABB& AABB2);

    Collision::AABB *aa;

};

#endif // COLLISIONSYSTEM_H
