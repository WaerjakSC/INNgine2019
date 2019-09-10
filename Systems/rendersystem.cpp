#include "rendersystem.h"
#include "renderview.h"
RenderSystem::RenderSystem() {
    factory = &ResourceManager::instance();
    // To-do: Implement signal/slot behavior to update the list of entities needed to render

    std::tie(mViableEntities, transforms, mats, meshes) = (factory->getRenderView()->getComponents()); // viable entities won't update in this class
    //    connectComponents();
}
void RenderSystem::iterateEntities() {
    for (int i = 0; i < mViableEntities.size(); i++) {
        transforms.at(i)->update();
        initializeOpenGLFunctions();
        glUseProgram(mats.at(i)->getShader()->getProgram());
        mats.at(i)->update();
        glBindVertexArray(meshes.at(i)->mVAO);
        if (meshes.at(i)->mIndiceCount > 0)
            glDrawElements(meshes.at(i)->mDrawType, meshes.at(i)->mIndiceCount, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(meshes.at(i)->mDrawType, 0, meshes.at(i)->mVerticeCount);
        meshes.at(i)->update();
    }
}

void RenderSystem::render() {
    iterateEntities();
}
