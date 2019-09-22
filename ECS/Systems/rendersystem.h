#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "resourcemanager.h"
#include <QOpenGLFunctions_4_1_Core>

class RenderSystem : public QObject, public QOpenGLFunctions_4_1_Core {
    Q_OBJECT
public:
    RenderSystem(std::map<ShaderType, Shader *> shaders);

    void render();

private:
    ResourceManager *factory{nullptr};

    void iterateEntities();
    void updateEntities();

    std::map<ShaderType, Shader *> mShaders;
    std::shared_ptr<Pool<MeshComponent>> mMeshPool;
    std::shared_ptr<Pool<MaterialComponent>> mMaterialPool;
    std::shared_ptr<Pool<TransformComponent>> mTransformPool;
};

#endif // RENDERSYSTEM_H
