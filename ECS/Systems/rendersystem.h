#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "pool.h"
#include "renderview.h"
#include <QOpenGLFunctions_4_1_Core>
#include <memory>
class ResourceManager;
class RenderView;
class RenderSystem : public QObject, public QOpenGLFunctions_4_1_Core {
    Q_OBJECT
public:
    RenderSystem(std::map<ShaderType, Shader *> shaders);

    void render();

    void init();
public slots:
    void changeShader(int entityID, ShaderType nShader);

private:
    ResourceManager *factory{nullptr};

    void iterateEntities();
    void updateEntities();

    std::map<ShaderType, Shader *> mShaders;
    std::unique_ptr<RenderView> mView;
};

#endif // RENDERSYSTEM_H
