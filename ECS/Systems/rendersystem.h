#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core.h"
#include "isystem.h"
#include "pool.h"
#include <QOpenGLFunctions_4_1_Core>
using namespace cjk;
class Registry;
class RenderSystem : public QObject, public ISystem, public QOpenGLFunctions_4_1_Core {
    Q_OBJECT
public:
    RenderSystem(std::map<std::string, Ref<Shader>> shaders);

    void update(DeltaTime dt = 0.016) override;

    void init();
public slots:
    void changeShader(int entityID, std::string nShader);

    void toggleRendered(GLuint entityID);

private:
    Registry *registry{nullptr};

    void iterateEntities();
    void updateEntities();

    std::map<std::string, Ref<Shader>> mShaders;
};

#endif // RENDERSYSTEM_H
