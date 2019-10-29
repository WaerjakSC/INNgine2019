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
    RenderSystem(std::map<std::string, Shader *> shaders);

    void update(float deltaTime = 0.016) override;

    void init();
public slots:
    void changeShader(int entityID, std::string nShader);

    void toggleRendered(GLuint entityID);

private:
    Registry *registry{nullptr};

    void iterateEntities();
    void updateEntities();

    std::map<std::string, Shader *> mShaders;
};

#endif // RENDERSYSTEM_H
