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
    RenderSystem();

    void update(DeltaTime dt = 0.016) override;

    void init();
public slots:
    /**
     * @brief changeShader Replace the entity's current shader with a new shader given by the editor GUI.
     * @param entityID
     * @param nShader String corresponding to the name of the new shader.
     */
    void changeShader(int entityID, std::string nShader);
    /**
     * @brief toggleRendered Toggle an entity's Mesh.IsRendered state.
     * @param entityID
     */
    void toggleRendered(GLuint entityID);

private:
    Registry *registry;
    /**
    * @brief drawEntities Render entities that want to be rendered (Mesh.isRendered).
    */
    void drawEntities();
    /**
     * @brief drawColliders Colliders have their own meshes, these are drawn with a plain shader and lines.
     * @todo Update to include Sphere colliders and other types.
     */
    void drawColliders();
};

#endif // RENDERSYSTEM_H
