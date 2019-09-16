#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "resourcemanager.h"
#include <QOpenGLFunctions_4_1_Core>

class RenderSystem : public QObject, public QOpenGLFunctions_4_1_Core {
    Q_OBJECT
public:
    RenderSystem(std::map<ShaderType, Shader *> shaders);

    void render();
public slots:
    void newEntity(std::tuple<int, TransformComponent *, MaterialComponent *, MeshComponent *> entity);

private:
    ResourceManager *factory{nullptr};
    std::vector<int> mViableEntities;
    std::vector<TransformComponent *> mTransforms;
    std::vector<MaterialComponent *> mMaterials;
    std::vector<MeshComponent *> mMeshes;
    std::map<ShaderType, Shader *> mShaders;
    void iterateEntities();
    void updateEntities();
};

#endif // RENDERSYSTEM_H
