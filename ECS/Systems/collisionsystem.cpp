#include "collisionsystem.h"
#include "registry.h"

CollisionSystem::CollisionSystem()
{

}

bool CollisionSystem::IntersectAABB(int entityID, int otherEntityID){
    // Get transforms of both objects
    reg->getComponent<Transform>(entityID);
    reg->getComponent<Transform>(otherEntityID);

    // Intersection: inspect the min and max coordinates of the boxes along each axis.
    // A[Xmin, Xmax] , B[Xmin, Xmax] and corresponding intervals for y and z-axis
    // Collision only if intervals overlap along ALL THREE axis
}


