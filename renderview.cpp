#include "renderview.h"

RenderView::RenderView(Pool<MeshComponent> *mesh, Pool<MaterialComponent> *mat, Pool<TransformComponent> *tf)
    : meshpool(mesh), matpool(mat), transpool(tf) {
}
CType RenderView::getSmallestPool() {
    bool meshS{true}; // I acknowledge this is a sucky way to check
    bool matS{false};
    size_t smallest = meshpool->size(); // start with the smallest being one of the pools
    if (matpool->size() < smallest) {   // If it turns out matpool is smaller, set it to that one and set matS to true
        meshS = false;
        matS = true;
        smallest = transpool->size();
    };
    if (transpool->size() < smallest) { // If transpool is smaller than either one, set meshS to false and matS to false
        return Transform;
    }
    if (matS)
        return Material;
    return Mesh;
}
std::vector<int> RenderView::getViableEntities() {
    std::vector<int> viableEntities;
    std::vector<int> entityList;
    if (getSmallestPool() == Mesh) {
        entityList = meshpool->getEntityList();
        for (auto entity : entityList) {                                                        // Iterate through the dense list of the smallest pool
            if (matpool->has(entityList.at(entity)) && transpool->has(entityList.at(entity))) { // If they exist in the other pools, add that entityID to the list of viable entities
                viableEntities.push_back(entity);
            }
        }
    } else if (getSmallestPool() == Material) {
        entityList = matpool->getEntityList();
        for (auto entity : entityList) {                                                         // Iterate through the dense list of the smallest pool
            if (meshpool->has(entityList.at(entity)) && transpool->has(entityList.at(entity))) { // If they exist in the other pools, add that entityID to the list of viable entities
                viableEntities.push_back(entity);
            }
        }
    } else {
        entityList = transpool->getEntityList();
        for (auto entity : entityList) {                                                       // Iterate through the dense list of the smallest pool
            if (matpool->has(entityList.at(entity)) && meshpool->has(entityList.at(entity))) { // If they exist in the other pools, add that entityID to the list of viable entities
                viableEntities.push_back(entity);
            }
        }
    }
    return viableEntities;
}

std::tuple<MeshComponent &, MaterialComponent &, TransformComponent &> RenderView::getComponents() {
}
