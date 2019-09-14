#include "rendersystem.h"
#include "Views/renderview.h"
RenderSystem::RenderSystem(std::map<ShaderType, Shader *> shaders) : mShaders(shaders) {
    factory = &ResourceManager::instance();
    // To-do: Implement signal/slot behavior to update the list of entities needed to render

    updateEntities();
    //    connectComponents();
}
void RenderSystem::iterateEntities() {
    for (size_t i = 0; i < mViableEntities.size(); i++) {
        transforms.at(i)->update();
        initializeOpenGLFunctions();
        ShaderType type = mats.at(i)->getShader();
        glUseProgram(mShaders[type]->getProgram());
        mShaders[type]->transmitUniformData(&transforms.at(i)->matrix(), &mats.at(i)->material);
        mats.at(i)->update();
        glBindVertexArray(meshes.at(i)->mVAO);
        meshes.at(i)->update();
        if (meshes.at(i)->mIndiceCount > 0)
            glDrawElements(meshes.at(i)->mDrawType, meshes.at(i)->mIndiceCount, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(meshes.at(i)->mDrawType, 0, meshes.at(i)->mVerticeCount);
    }
}

void RenderSystem::render() {
    iterateEntities();
}
void RenderSystem::updateEntities() {
    std::tie(mViableEntities, transforms, mats, meshes) = (factory->getRenderView()->getComponents());
}
