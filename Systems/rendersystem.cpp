#include "rendersystem.h"

RenderSystem::RenderSystem() {
    factory = &ResourceManager::instance();
    mRenderCompIDs.emplace_back(factory->getCompIndex().at(Transform));
    mRenderCompIDs.emplace_back(factory->getCompIndex().at(Material));
    mRenderCompIDs.emplace_back(factory->getCompIndex().at(Mesh));
    // To-do: Implement signal/slot behavior to update the list of entities needed to render
    mViableEntities = getViableEntities();
    connectComponents();
}
void RenderSystem::iterateEntity(int eID) {
    for (auto cOffset : mRenderCompIDs) {
        int componentLocation = factory->getEntityStart().at(eID) + cOffset->at(eID);
        factory->getComponents().at(componentLocation)->update();
    }
}
/**
 * @brief RenderSystem::getViableEntities
 * @return a list of all the entities the system will want to iterate through in render()
 */
std::vector<int> RenderSystem::getViableEntities() {
    std::map<int, int> *smallestMap = mRenderCompIDs.at(0);
    std::vector<int> entityIDs;
    std::vector<int> compOffsets;
    for (auto compType : mRenderCompIDs) { // Get the smallest map -- This will contain all the entities most likely to contain all three components required.
        if (compType->size() < smallestMap->size())
            smallestMap = compType;
    }
    for (std::map<int, int>::iterator it = smallestMap->begin(); it != smallestMap->end(); ++it) {
        entityIDs.emplace_back(it->first); // Find the ID of each entity in the map
    }
    std::vector<int> renderEntities;
    for (size_t entity = 0; entity < entityIDs.size(); entity++) { // For each entity in the first map
        int dupes{0};
        for (size_t type = 0; type < mRenderCompIDs.size(); type++) {
            if (mRenderCompIDs.at(type) == smallestMap) // Don't search yourself
                continue;
            auto search = mRenderCompIDs.at(type)->find(entityIDs.at(entity));
            if (search == mRenderCompIDs.at(type)->end()) {
                break; // If you don't find a duplicate, you can discard this loop.
            } else {
                dupes++;
            }
        }
        if (dupes == 2) { // Dupes found in both other maps -- this means rendersystem can run it
            renderEntities.push_back(entityIDs.at(entity));
        }
    }
    return renderEntities;
}

void RenderSystem::render() {
    for (auto entityID : mViableEntities) {
        iterateEntity(entityID);
    }
}
/**
 * @brief RenderSystem::connectComponents VERY temporary function just to make things render
 */
void RenderSystem::connectComponents() {
    for (auto entity : mViableEntities) {
        Shader *shader = static_cast<MaterialComponent *>(factory->getComponent(Material, entity))->getShader();
        gsl::Matrix4x4 model = static_cast<TransformComponent *>(factory->getComponent(Transform, entity))->matrix();
        static_cast<MaterialComponent *>(factory->getComponent(Material, entity))->setMatrix(model);
        static_cast<MeshComponent *>(factory->getComponent(Mesh, entity))->setShader(shader);
    }
}
