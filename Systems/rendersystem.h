#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "resourcemanager.h"
#include <QOpenGLFunctions_4_1_Core>

class RenderSystem : public QOpenGLFunctions_4_1_Core {
public:
    RenderSystem(std::map<ShaderType, Shader *> shaders);

    void render();

private:
    ResourceManager *factory{nullptr};
    std::vector<int> mViableEntities;
    std::vector<TransformComponent *> transforms;
    std::vector<MaterialComponent *> mats;
    std::vector<MeshComponent *> meshes;
    std::map<ShaderType, Shader *> mShaders;
    void iterateEntities();
};

#endif // RENDERSYSTEM_H
