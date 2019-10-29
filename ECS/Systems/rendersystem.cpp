#include "rendersystem.h"
#include "billboard.h"
#include "colorshader.h"
#include "components.h"
#include "phongshader.h"
#include "registry.h"
#include "textureshader.h"
#include "view.h"
RenderSystem::RenderSystem(std::map<std::string, Shader *> shaders) : mShaders(shaders) {
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
}
void RenderSystem::init() {
    iterateEntities();
}
void RenderSystem::update(float deltaTime) {
    Q_UNUSED(deltaTime);
    iterateEntities();
}
void RenderSystem::toggleRendered(GLuint entityID) {
    bool &isRendered = registry->view<Mesh>().get(entityID).mRendered;
    isRendered = !isRendered;
}
void RenderSystem::changeShader(int entityID, std::string nShader) {
    Shader *shader{nullptr};
    ResourceManager *factory = ResourceManager::instance();
    if (nShader == "colorshader")
        shader = factory->getShader<ColorShader>();
    else if (nShader == "textureshader")
        shader = factory->getShader<TextureShader>();
    else if (nShader == "phongshader")
        shader = factory->getShader<PhongShader>();
    registry->view<Material>().get(entityID).mShader = shader;
}
