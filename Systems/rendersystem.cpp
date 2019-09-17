#include "rendersystem.h"
#include "Views/renderview.h"
RenderSystem::RenderSystem(std::map<ShaderType, Shader *> shaders) : mShaders(shaders) {
    factory = ResourceManager::instance();
    // To-do: Implement signal/slot behavior to update the list of entities needed to render

    updateEntities();
    //    connectComponents();
}
// To-do: Render only entities that want to be rendered.
void RenderSystem::iterateEntities() {
    for (size_t i = 0; i < mViableEntities.size(); i++) {
        mTransforms.at(i)->update();
        initializeOpenGLFunctions();
        ShaderType type = mMaterials.at(i)->getShader();
        glUseProgram(mShaders[type]->getProgram());
        mShaders[type]->transmitUniformData(&mTransforms.at(i)->matrix(), &mMaterials.at(i)->material);
        mMaterials.at(i)->update();
        glBindVertexArray(mMeshes.at(i)->mVAO);
        mMeshes.at(i)->update();
        if (mMeshes.at(i)->mIndiceCount > 0)
            glDrawElements(mMeshes.at(i)->mDrawType, mMeshes.at(i)->mIndiceCount, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(mMeshes.at(i)->mDrawType, 0, mMeshes.at(i)->mVerticeCount);
    }
}

void RenderSystem::render() {
    iterateEntities();
}
void RenderSystem::updateEntities() {
    std::tie(mViableEntities, mTransforms, mMaterials, mMeshes) = (factory->getRenderView()->getComponents());
}
void RenderSystem::newEntity(std::tuple<int, TransformComponent *, MaterialComponent *, MeshComponent *> entity) {
    mViableEntities.emplace_back(std::get<0>(entity));
    mTransforms.emplace_back(std::get<1>(entity));
    mMaterials.emplace_back(std::get<2>(entity));
    mMeshes.emplace_back(std::get<3>(entity));
}
