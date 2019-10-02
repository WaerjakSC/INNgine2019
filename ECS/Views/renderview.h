#ifndef RENDERVIEW_H
#define RENDERVIEW_H
#include "pool.h"
#include <memory>
#include <vector>
class RenderView : public QObject {
    Q_OBJECT
public:
    RenderView();

private:
    size_t lastRenderItem{0};
    std::shared_ptr<Pool<Mesh>> mMeshPool;
    std::shared_ptr<Pool<Material>> mMaterialPool;
    std::shared_ptr<Pool<Transform>> mTransformPool;
    void sortGroup();
};

#endif // RENDERVIEW_H
