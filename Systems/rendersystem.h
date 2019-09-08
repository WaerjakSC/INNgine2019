#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "Components/materialcomponent.h"
#include "Components/meshcomponent.h"
#include "resourcemanager.h"
#include <QOpenGLFunctions_4_1_Core>

class RenderSystem : public QOpenGLFunctions_4_1_Core {
public:
    RenderSystem();
    // Each map in mCompIndex contains a list of entities with that component
    // Get the smallest map of componentIndices and check for overlaps in the other maps
    // RenderSystem needs:
    // Access to Mesh, Material, Transform (or at least mMatrix)
    // mEntityStart might need to be a static object or something
    void render();

private:
    std::vector<std::map<int, int> *> mRenderCompIDs;
    ResourceManager *factory{nullptr};
    std::vector<int> mViableEntities;
    std::vector<int> getViableEntities();
    void iterateEntity(int eID);
    void connectComponents();
};

#endif // RENDERSYSTEM_H
