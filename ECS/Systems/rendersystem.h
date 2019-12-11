#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "camera.h"
#include "core.h"
#include "isystem.h"
#include "pool.h"
#include <QOpenGLFunctions_4_1_Core>

class Registry;
/**
 * @brief The RenderSystem class draws all objects containing at least Transform, Material and Mesh components.
 */
class RenderSystem : public QObject, public ISystem, public QOpenGLFunctions_4_1_Core {
    Q_OBJECT
public:
    RenderSystem();

    void update(DeltaTime = 0.016) override;
    void updateEditorOnly();

    void setSkyBoxID(const GLuint &skyBoxID);

public slots:
    /**
     * @brief Replace the entity's current shader with a new shader given by the editor GUI.
     * @param entityID
     * @param nShader String corresponding to the name of the new shader.
     */
    void changeShader(const QString &nShader);
    /**
     * @brief Toggle an entity's Mesh.IsRendered state.
     * @param entityID
     */
    void toggleRendered(GLuint entityID);
    /**
     * @brief Set the rendered state of the entity.
     * @param entityID
     * @param nState
     */
    void setRendered(GLuint entityID, bool nState);
signals:
    void newRenderedSignal(GLuint entityID, Qt::CheckState nState);

private:
    Registry *registry;
    /**
    * @brief Render entities that want to be rendered (Mesh.isRendered).
    */
    void drawEntities();
    /**
     * @brief Colliders have their own meshes, these are drawn with a plain shader and lines.
     */
    void drawColliders();

    GLuint mSkyBoxID;
    /**
     * @brief Draw the skybox with its own shader and OpenGL settings.
     */
    void drawSkybox();
    void Cull(const Camera::Frustum &f);
};

#endif // RENDERSYSTEM_H
