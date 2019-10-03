#include "rendersystem.h"
#include "billboard.h"
#include "registry.h"
#include "resourcemanager.h"
RenderSystem::RenderSystem(std::map<ShaderType, Shader *> shaders) : mShaders(shaders) {
    factory = ResourceManager::instance();
    mView = std::make_unique<RenderView>();
}
/**
 * @brief RenderSystem::iterateEntities
 * @todo Render only entities that want to be rendered.
 * addendum - sort mesh and material pool by whether they are visible or not - hold an iterator that denotes the end of the "watched" group
 */
void RenderSystem::iterateEntities() {
    // Iterate through entities -- We know Mesh and Material types will always want to be rendered, so we can iterate through all of the entities that own those types.
    for (size_t curEntity = 0; curEntity < mView->mTransformPool->groupEnd(); curEntity++) {
        auto &mesh = mView->mMeshPool->data()[curEntity]; // We can access the component directly without any indirections because RenderSystem owns mesh and material types
        auto &material = mView->mMaterialPool->data()[curEntity];
        auto &transform = mView->mTransformPool->data()[curEntity]; // Transforms could be sorted some unknown way - therefore we need to find the index of the component first through get(int entityID)
        ShaderType type = material.mShader;

        initializeOpenGLFunctions();
        glUseProgram(mShaders[type]->getProgram());
        mShaders[type]->transmitUniformData(&transform.mMatrix, &material);
        glBindVertexArray(mesh.mVAO);
        if (mesh.mIndiceCount > 0)
            glDrawElements(mesh.mDrawType, mesh.mIndiceCount, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(mesh.mDrawType, 0, mesh.mVerticeCount);
    }
    for (auto billBoard : factory->billBoards()) {
        auto &transform = mView->mTransformPool->get(billBoard);
        ShaderType type = mView->mMaterialPool->get(billBoard).mShader;
        dynamic_cast<BillBoard *>(factory->getGameObject(billBoard))->update(&transform, mShaders[type]);
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
