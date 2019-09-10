#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Components/materialcomponent.h"
#include "Components/meshcomponent.h"
#include "resourcemanager.h"
#include <QOpenGLFunctions_4_1_Core>

class RenderSystem : public QOpenGLFunctions_4_1_Core {
public:
    RenderSystem();

    void render();

private:
    ResourceManager *factory{nullptr};
    std::vector<int> mViableEntities;
    std::vector<TransformComponent *> transforms;
    std::vector<MaterialComponent *> mats;
    std::vector<MeshComponent *> meshes;
    void iterateEntities();
};

#endif // RENDERSYSTEM_H
