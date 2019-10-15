#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "pool.h"
#include "renderview.h"
#include <QOpenGLFunctions_4_1_Core>
#include <memory>
class Registry;
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
    Registry *registry{nullptr};

    void iterateEntities();
    void updateEntities();

    std::map<ShaderType, Shader *> mShaders;
    std::unique_ptr<RenderView> mView;
    std::shared_ptr<Pool<Collision>> mColliderPool;
};

#endif // RENDERSYSTEM_H
