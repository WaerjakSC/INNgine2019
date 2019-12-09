#include "rendersystem.h"
#include "cameracontroller.h"
#include "colorshader.h"
#include "components.h"
#include "group.h"
#include "inputsystem.h"
#include "phongshader.h"
#include "registry.h"
#include "skyboxshader.h"
#include "textureshader.h"
#include "view.h"

RenderSystem::RenderSystem() : registry{Registry::instance()}
{
    [[maybe_unused]] auto group{registry->group<Transform, Material, Mesh>()}; // Creating a group early reduces initial cost of first-time creation.
}

void RenderSystem::drawEntities()
{
    initializeOpenGLFunctions();
    // Iterate entities. View returns only the entities that own all the given types so it should be safe to iterate all of them equally.
    auto group{registry->group<Transform, Material, Mesh>()};
    for (auto entity : group) {
        auto [transform, material, mesh]{group.get<Transform, Material, Mesh>(entity)}; // Structured bindings (c++17), creates and assigns from tuple
        if (mesh.mRendered) {
            glUseProgram(material.mShader->getProgram());
            material.mShader->transmitUniformData(transform.modelMatrix, &material);
            glBindVertexArray(mesh.mVAO);
            if (mesh.mIndiceCount > 0)
                glDrawElements(mesh.mDrawType, mesh.mIndiceCount, GL_UNSIGNED_INT, nullptr);
            else
                glDrawArrays(mesh.mDrawType, 0, mesh.mVerticeCount);
        }
    }
    drawSkybox();
}
void RenderSystem::update(DeltaTime)
{
    drawEntities();
}

void RenderSystem::updateEditorOnly()
{
    drawColliders();
}

void RenderSystem::drawSkybox()
{
    auto skyBoxview{registry->view<Material, Mesh>()};
    auto [material, mesh]{skyBoxview.get<Material, Mesh>(mSkyBoxID)};
    glDepthFunc(GL_LEQUAL);
    glUseProgram(material.mShader->getProgram());
    gsl::Matrix4x4 temp;
    material.mShader->transmitUniformData(temp, &material);
    glBindVertexArray(mesh.mVAO);
    glDrawElements(mesh.mDrawType, mesh.mIndiceCount, GL_UNSIGNED_INT, nullptr);
    glDepthFunc(GL_LESS);
}
void RenderSystem::drawColliders()
{
    auto view{registry->view<AABB>()};
    ColorShader *shader{ResourceManager::instance()->getShader<ColorShader>().get()};
    for (auto entity : view) {
        auto &aabb{view.get(entity)};
        glUseProgram(shader->getProgram());
        // For AABB you could possibly alter the modelMatrix by a desired position or scale(half-size) before sending it to the shader.
        shader->transmitUniformData(aabb.transform.modelMatrix, nullptr); // no need to send a material since the box collider is just lines
        glBindVertexArray(aabb.colliderMesh.mVAO);
        glDrawArrays(aabb.colliderMesh.mDrawType, 0, aabb.colliderMesh.mVerticeCount);
    }
}
/**
 * @brief RenderWindow::Cull, Cull furries right now
 * @param f, the frustum
 */
void RenderSystem::Cull(const Camera::Frustum &f)
{
    auto view{registry->view<Mesh, AABB>()};
    auto inputSystem{registry->system<InputSystem>()};
    Camera &cam{inputSystem->editorCamController()->getCamera()};
    for (auto entity : view) {
        auto &collider{view.get<AABB>(entity)};
        if (cam.getFrustum().Intersects(f, collider)) {
            view.get<Mesh>(entity).mRendered = true;
        }
        else
            view.get<Mesh>(entity).mRendered = false;
    }
}
void RenderSystem::setSkyBoxID(const GLuint &skyBoxID)
{
    mSkyBoxID = skyBoxID;
}
void RenderSystem::toggleRendered(GLuint entityID)
{
    bool &isRendered{registry->view<Mesh>().get(entityID).mRendered};
    isRendered = !isRendered;
}
void RenderSystem::changeShader(const QString &nShader)
{
    GLuint eID{registry->getSelectedEntity()};
    cjk::Ref<Shader> shader{nullptr};
    ResourceManager *factory{ResourceManager::instance()};
    if (nShader == "PlainShader")
        registry->view<Material>().get(eID).mShader = factory->getShader<ColorShader>();
    else if (nShader == "TextureShader")
        registry->view<Material>().get(eID).mShader = factory->getShader<TextureShader>();
    else if (nShader == "PhongShader")
        registry->view<Material>().get(eID).mShader = factory->getShader<PhongShader>();
    else if (nShader == "SkyboxShader")
        registry->view<Material>().get(eID).mShader = factory->getShader<SkyboxShader>();
}

