#include "rendersystem.h"
#include "billboard.h"
#include "registry.h"
#include "view.h"
#include "components.h"
RenderSystem::RenderSystem(std::map<ShaderType, Shader *> shaders) : mShaders(shaders) {
    registry = Registry::instance();
    mView = std::make_unique<RenderView>();
    //    View<Transform, Material, Mesh> test(mView->mTransformPool, mView->mMaterialPool, mView->mMeshPool);
    //    auto [transform, material] = test.get<Transform, Material>(1);
    //    qDebug() << transform.position;
    mColliderPool = registry->registerComponent<Collision>();
}
/**
 * @brief RenderSystem::iterateEntities
 * @todo Render only entities that want to be rendered.
 * addendum - sort mesh and material pool by whether they are visible or not - hold an iterator that denotes the end of the "watched" group
 */
void RenderSystem::iterateEntities() {
    // Iterate through entities -- We know Mesh and Material types will always want to be rendered, so we can iterate through all of the entities that own those types.
    for (size_t listIndex = 0; listIndex < mView->mTransformPool->groupEnd(); listIndex++) {
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
        // Doesnt work yet
        if(registry->contains(listIndex, CType::Collision)){
            auto &collider = mColliderPool->data()[listIndex];
            glBindVertexArray(0);
            glBindVertexArray(collider.mVAO);
            glDrawElements(GL_LINE_LOOP, 16, GL_UNSIGNED_SHORT, nullptr); // Might use GL_STRIP instead, not sure yet
        }
    }
}
void RenderSystem::init() {
    mView->sortGroup();
}
void RenderSystem::update(float deltaTime) {
    Q_UNUSED(deltaTime);
    iterateEntities();
}

void RenderSystem::changeShader(int entityID, ShaderType nShader) {
    mView->mMaterialPool->get(entityID).mShader = nShader;
}
