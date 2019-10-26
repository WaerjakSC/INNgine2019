#include "rendersystem.h"
#include "billboard.h"
#include "components.h"
#include "registry.h"
#include "view.h"
RenderSystem::RenderSystem(std::map<ShaderType, Shader *> shaders) : mShaders(shaders) {
    registry = Registry::instance();
}
/**
 * @brief RenderSystem::iterateEntities
 * @todo Render only entities that want to be rendered.
 * addendum - sort mesh and material pool by whether they are visible or not - hold an iterator that denotes the end of the "watched" group
 */
void RenderSystem::iterateEntities() {
    // Iterate entities. View returns only the entities that own all the given types so it should be safe to iterate all of them equally.
    auto view = registry->view<Transform, Material, Mesh>();
    for (auto entity : view) {
        auto [transform, material, mesh] = view.get<Transform, Material, Mesh>(entity); // Structured bindings (c++17), creates and assigns from tuple

        ShaderType type = material.mShader;

        initializeOpenGLFunctions();
        glUseProgram(mShaders[type]->getProgram());
        mShaders[type]->transmitUniformData(transform.modelMatrix, &material);
        glBindVertexArray(mesh.mVAO);
        if (mesh.mIndiceCount > 0)
            glDrawElements(mesh.mDrawType, mesh.mIndiceCount, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(mesh.mDrawType, 0, mesh.mVerticeCount);
    }
    // Doesnt work yet -- Move to CollisionSystem imo
    //        if (registry->contains(listIndex, CType::Collision)) {
    //            auto &collider = mColliderPool->data()[listIndex];
    //            glBindVertexArray(0);
    //            glBindVertexArray(collider.mVAO);
    //            glDrawElements(GL_LINE_LOOP, 16, GL_UNSIGNED_SHORT, nullptr); // Might use GL_STRIP instead, not sure yet
    //        }
}
void RenderSystem::init() {
    iterateEntities();
}
void RenderSystem::update(float deltaTime) {
    Q_UNUSED(deltaTime);
    iterateEntities();
}

void RenderSystem::changeShader(int entityID, ShaderType nShader) {
    registry->view<Material>().get(entityID).mShader = nShader;
}
