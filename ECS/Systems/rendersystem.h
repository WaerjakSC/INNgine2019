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
    void changeShader(int entityID, ShaderType nShader);

private:
    ResourceManager *factory{nullptr};

    void iterateEntities();
    void updateEntities();

    std::map<ShaderType, Shader *> mShaders;
    std::shared_ptr<Pool<Mesh>> mMeshPool;
    std::shared_ptr<Pool<Material>> mMaterialPool;
    std::shared_ptr<Pool<Transform>> mTransformPool;
};

#endif // RENDERSYSTEM_H
