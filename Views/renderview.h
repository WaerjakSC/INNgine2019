#ifndef RENDERVIEW_H
#define RENDERVIEW_H
#include "pool.h"
#include <vector>

class RenderView {
public:
    RenderView(Pool<TransformComponent> *tf, Pool<MaterialComponent> *mat, Pool<MeshComponent> *mesh);

    std::vector<int> getViableEntities();
    std::tuple<std::vector<int>, std::vector<TransformComponent *>, std::vector<MaterialComponent *>, std::vector<MeshComponent *>> getComponents();
    // RenderSystem View? Get all entities that own these three component types
private:
    Pool<MeshComponent> *meshpool;
    Pool<MaterialComponent> *matpool;
    Pool<TransformComponent> *transpool;
    std::vector<int> mViableEntities;
    CType getSmallestPool();
};

#endif // RENDERVIEW_H
