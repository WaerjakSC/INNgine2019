#ifndef RENDERVIEW_H
#define RENDERVIEW_H
#include "Components/meshcomponent.h"
#include "Components/transformcomponent.h"
#include "pool.h"
#include <vector>

class RenderView {
public:
    RenderView(Pool<MeshComponent> *mesh, Pool<MaterialComponent> *mat, Pool<TransformComponent> *tf);

    std::vector<int> getViableEntities();
    std::tuple<MeshComponent &, MaterialComponent &, TransformComponent &> getComponents();
    // RenderSystem View? Get all entities that own these three component types
private:
    Pool<MeshComponent> *meshpool;
    Pool<MaterialComponent> *matpool;
    Pool<TransformComponent> *transpool;
    CType getSmallestPool();
};

#endif // RENDERVIEW_H
