#ifndef RENDERVIEW_H
#define RENDERVIEW_H
#include "Components/meshcomponent.h"
#include "Components/transformcomponent.h"
#include "pool.h"
#include <vector>

class RenderView {
public:
    RenderView();

    // RenderSystem View? Get all entities that own these three component types
private:
    std::vector<Pool<MeshComponent> *> meshpool;
    std::vector<Pool<MaterialComponent> *> matpool;
    std::vector<Pool<TransformComponent> *> transpool;
};

#endif // RENDERVIEW_H
