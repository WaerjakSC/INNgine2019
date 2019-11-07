#include "rendersystem.h"
#include "billboard.h"
#include "colorshader.h"
#include "components.h"
#include "phongshader.h"
#include "registry.h"
#include "textureshader.h"
#include "view.h"
RenderSystem::RenderSystem(std::map<std::string, Ref<Shader>> shaders) : mShaders(shaders) {
    registry = Registry::instance();
}
/**
 * @brief RenderSystem::iterateEntities
 * @todo Render only entities that want to be rendered.
 * addendum - sort mesh and material pool by whether they are visible or not - hold an iterator that denotes the end of the "watched" group
 */
void RenderSystem::iterateEntities() {
    initializeOpenGLFunctions();
    // Iterate entities. View returns only the entities that own all the given types so it should be safe to iterate all of them equally.
    auto view = registry->view<Transform, Material, Mesh>();
    for (auto entity : view) {
        auto [transform, material, mesh] = view.get<Transform, Material, Mesh>(entity); // Structured bindings (c++17), creates and assigns from tuple
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
    if (!ResourceManager::instance()->isPlaying()) {
        auto colliderView = registry->view<AABB>();
        ColorShader *shader = ResourceManager::instance()->getShader<ColorShader>().get();
        for (auto entity : colliderView) {
            auto &aabb = colliderView.get(entity);
            glUseProgram(shader->getProgram());
            // For AABB you could possibly alter the modelMatrix by a desired position or scale(half-size) before sending it to the shader.
            shader->transmitUniformData(aabb.transform.modelMatrix, nullptr); // no need to send a material since the box collider is just lines
            glBindVertexArray(aabb.colliderMesh.mVAO);
            glDrawArrays(aabb.colliderMesh.mDrawType, 0, aabb.colliderMesh.mVerticeCount);
        }
    }
}
void RenderSystem::init() {
    iterateEntities();
}
void RenderSystem::update(DeltaTime dt) {
    Q_UNUSED(dt);
    iterateEntities();
}
void RenderSystem::toggleRendered(GLuint entityID) {
    bool &isRendered = registry->view<Mesh>().get(entityID).mRendered;
    isRendered = !isRendered;
}
void RenderSystem::changeShader(int entityID, std::string nShader) {
    Ref<Shader> shader{nullptr};
    ResourceManager *factory = ResourceManager::instance();
    if (nShader == "PlainShader")
        registry->view<Material>().get(entityID).mShader = factory->getShader<ColorShader>();
    else if (nShader == "TextureShader")
        registry->view<Material>().get(entityID).mShader = factory->getShader<TextureShader>();
    else if (nShader == "PhongShader")
        registry->view<Material>().get(entityID).mShader = factory->getShader<PhongShader>();
}
