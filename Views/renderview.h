#ifndef RENDERVIEW_H
#define RENDERVIEW_H
#include "pool.h"
#include <vector>

class RenderView : public QObject {
    Q_OBJECT
public:
    RenderView(Pool<TransformComponent> *tf, Pool<MaterialComponent> *mat, Pool<MeshComponent> *mesh);

    std::vector<int> getViableEntities();
    std::tuple<std::vector<int>, std::vector<TransformComponent *>, std::vector<MaterialComponent *>, std::vector<MeshComponent *>> getComponents();
    // RenderSystem View? Get all entities that own these three component types
public slots:
    void addEntity(int entityID);
signals:
    void updateSystem(std::tuple<int, TransformComponent *, MaterialComponent *, MeshComponent *>);

private:
    Pool<MeshComponent> *mMeshPool;
    Pool<MaterialComponent> *mMaterialPool;
    Pool<TransformComponent> *mTransformPool;
    std::vector<int> mViableEntities;
    CType getSmallestPool();
};

#endif // RENDERVIEW_H
