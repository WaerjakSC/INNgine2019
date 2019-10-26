#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "isystem.h"
#include "pool.h"
#include <QOpenGLFunctions_4_1_Core>
#include <memory>
class Registry;
class RenderSystem : public QObject, public ISystem, public QOpenGLFunctions_4_1_Core {
    Q_OBJECT
public:
    RenderSystem(std::map<ShaderType, Shader *> shaders);

    void update(float deltaTime = 0.016) override;

    void init();
public slots:
    void changeShader(int entityID, ShaderType nShader);

private:
    Registry *registry{nullptr};

    void iterateEntities();
    void updateEntities();

    std::map<ShaderType, Shader *> mShaders;
};

#endif // RENDERSYSTEM_H
