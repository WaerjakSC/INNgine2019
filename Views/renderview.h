#ifndef RENDERVIEW_H
#define RENDERVIEW_H
#include "pool.h"
#include <memory>
#include <vector>
class RenderView : public QObject {
    Q_OBJECT
public:
    RenderView(std::shared_ptr<Pool<TransformComponent>> tf);

    std::vector<int> getViableEntities();
    std::tuple<std::vector<int>, std::vector<TransformComponent *>, std::vector<MaterialComponent *>, std::vector<MeshComponent *>> getComponents();

    Pool<MeshComponent> *getMeshPool() const;
    Pool<MaterialComponent> *getMaterialPool() const;
public slots:
    void addEntity(int entityID);
signals:
    void updateSystem(std::tuple<int, TransformComponent *, MaterialComponent *, MeshComponent *>);

private:
    std::unique_ptr<Pool<MeshComponent>> mMeshPool;
    std::unique_ptr<Pool<MaterialComponent>> mMaterialPool;
    std::shared_ptr<Pool<TransformComponent>> mTransformPool;
    std::vector<int> mViableEntities;
    CType getSmallestPool();
};

#endif // RENDERVIEW_H
