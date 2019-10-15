#include "rendersystem.h"
#include "billboard.h"
#include "registry.h"
RenderSystem::RenderSystem(std::map<ShaderType, Shader *> shaders) : mShaders(shaders) {
    registry = Registry::instance();
    mView = std::make_unique<RenderView>();
    mColliderPool = registry->registerComponent<Collision>();
}
/**
 * @brief RenderSystem::iterateEntities
 * @todo Render only entities that want to be rendered.
 * addendum - sort mesh and material pool by whether they are visible or not - hold an iterator that denotes the end of the "watched" group
 */
void RenderSystem::iterateEntities() {
    GLuint listIndex{0};
    // Iterate through entities -- We know Mesh and Material types will always want to be rendered, so we can iterate through all of the entities that own those types.
    for (auto entity : mView->mMeshPool->entities())
    /*   for (size_t listIndex = 0; listIndex < mView->mTransformPool->groupEnd(); listIndex++)*/ {
        auto &mesh = mView->mMeshPool->data()[listIndex]; // We can access the component directly without any indirections because RenderSystem owns mesh and material types
        auto &material = mView->mMaterialPool->data()[listIndex];
        auto &transform = mView->mTransformPool->data()[listIndex];
        ShaderType type = material.mShader;

        initializeOpenGLFunctions();
        glUseProgram(mShaders[type]->getProgram());
        mShaders[type]->transmitUniformData(transform.modelMatrix, &material);
        glBindVertexArray(mesh.mVAO);
        if (mesh.mIndiceCount > 0)
            glDrawElements(mesh.mDrawType, mesh.mIndiceCount, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(mesh.mDrawType, 0, mesh.mVerticeCount);
        if (registry->contains(entity, CType::Collision)) { // PS: At the moment only entities with all 4 component types have their collision boxes rendered (so pure collision boxes won't work)
            auto &collider = mColliderPool->get(entity);
            glBindVertexArray(0);
            //            glBindVertexArray(collider.mVAO);
            //            glDrawArrays(GL_LINE_STRIP, 0, collider.mVertices.size()); // With GL_LINE_STRIP it should be enough to create the points of a cube (or whichever square collider)
        }
        listIndex++;
    }
}
void RenderSystem::init() {
    mView->sortGroup();
}
void RenderSystem::render() {
    iterateEntities();
}

void RenderSystem::changeShader(int entityID, ShaderType nShader) {
    mView->mMaterialPool->get(entityID).mShader = nShader;
}
