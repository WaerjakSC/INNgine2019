#include "renderview.h"
RenderView::RenderView(Pool<TransformComponent> *tf, Pool<MaterialComponent> *mat, Pool<MeshComponent> *mesh)
    : meshpool(mesh), matpool(mat), transpool(tf) {
}
CType RenderView::getSmallestPool() {
    if (*matpool <= *meshpool) // If material pool is smaller than or equal to mesh pool
    {
        if (*matpool <= *transpool) // If also smaller than transpool
            return Material;
        else
            return Transform;           // If not smaller than transpool, transpool must be smallest.
    } else if (*meshpool <= *transpool) // If material pool is larger than mesh pool, check if mesh pool is smaller than or equal to transform pool
    {
        return Mesh;
    } else
        return Transform; // If the two above checks fail then just return the transform pool.
}
void RenderView::getViableEntities() {
    std::vector<int> entityList;
    mViableEntities.clear();
    if (getSmallestPool() == Mesh) {
        entityList = meshpool->getEntityList();
        for (auto entity : entityList) {                                                        // Iterate through the dense list of the smallest pool
            if (matpool->has(entityList.at(entity)) && transpool->has(entityList.at(entity))) { // If they exist in the other pools, add that entityID to the list of viable entities
                mViableEntities.push_back(entity);
            }
        }
    } else if (getSmallestPool() == Material) {
        entityList = matpool->getEntityList();
        for (auto entity : entityList) {
            if (meshpool->has(entityList.at(entity)) && transpool->has(entityList.at(entity))) {
                mViableEntities.push_back(entity);
            }
        }
    } else {
        entityList = transpool->getEntityList();
        for (auto entity : entityList) {
            if (matpool->has(entityList.at(entity)) && meshpool->has(entityList.at(entity))) {
                mViableEntities.push_back(entity);
            }
        }
    }
}

/**
 * @brief Update RenderView with the new entity
 * @param entityID
 * @return
 */
void RenderView::addEntity(int entityID) {
    mViableEntities.emplace_back(entityID);
    emit updateSystem(std::make_tuple(entityID, transpool->get(entityID), matpool->get(entityID), meshpool->get(entityID)));
}
std::tuple<std::vector<int>, std::vector<TransformComponent *>, std::vector<MaterialComponent *>, std::vector<MeshComponent *>> RenderView::getComponents() {
    std::vector<MeshComponent *> meshes;
    std::vector<MaterialComponent *> mats;
    std::vector<TransformComponent *> transforms;
    getViableEntities();
    for (auto entity : mViableEntities) {
        transforms.emplace_back(transpool->get(entity));
        mats.emplace_back(matpool->get(entity));
        meshes.emplace_back(meshpool->get(entity));
    }
    return std::make_tuple(mViableEntities, transforms, mats, meshes);
}
