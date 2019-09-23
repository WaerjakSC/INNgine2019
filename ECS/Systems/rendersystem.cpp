#include "rendersystem.h"
#include "billboard.h"
#include "registry.h"
#include "renderview.h"
RenderSystem::RenderSystem(std::map<ShaderType, Shader *> shaders) : mShaders(shaders) {
    factory = ResourceManager::instance();
    mMaterialPool = std::make_shared<Pool<Material>>();
    Registry::instance()->registerComponent<Material>(mMaterialPool);
    mMeshPool = std::make_shared<Pool<Mesh>>();
    Registry::instance()->registerComponent<Mesh>(mMeshPool);
    mTransformPool = Registry::instance()->registerComponent<Transform>();
}
/**
 * @brief RenderSystem::iterateEntities
 * @todo Render only entities that want to be rendered.
 * addendum - sort mesh and material pool by whether they are visible or not - hold an iterator that denotes the end of the "watched" group
 */
void RenderSystem::iterateEntities() {
    GLuint curEntity{0}; // which index of entities() is being iterated
    // Iterate through entities -- We know Mesh and Material types will always want to be rendered, so we can iterate through all of the entities that own those types.
    for (auto entityID : mMeshPool->entities()) {
        auto &mesh = mMeshPool->data()[curEntity]; // We can access the component directly without any indirections because RenderSystem owns mesh and material types
        auto &material = mMaterialPool->data()[curEntity];
        auto &transform = mTransformPool->get(entityID); // Transforms could be sorted some unknown way - therefore we need to find the index of the component first through get(int entityID)
        ShaderType type = material.getShader();

        initializeOpenGLFunctions();
        transform.update();
        // If entity is a billboard, additionally run the update function for that
        BillBoard *billboard = dynamic_cast<BillBoard *>(factory->getGameObject(entityID));
        if (billboard)
            billboard->update(&transform, mShaders[type]); // This probably causes some performance problems but idk how else to do this atm
        glUseProgram(mShaders[type]->getProgram());
        mShaders[type]->transmitUniformData(&transform.matrix(), &material.material);
        glBindVertexArray(mesh.mVAO);
        material.update();
        mesh.update();
        if (mesh.mIndiceCount > 0)
            glDrawElements(mesh.mDrawType, mesh.mIndiceCount, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(mesh.mDrawType, 0, mesh.mVerticeCount);
        curEntity++;
    }
}

void RenderSystem::render() {
    iterateEntities();
}
