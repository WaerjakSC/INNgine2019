#include "renderview.h"
#include "registry.h"
RenderView::RenderView(/*std::shared_ptr<Pool<TransformComponent>> tf*/) {
    //    mMaterialPool = std::make_unique<Pool<MaterialComponent>>();
    //    mMeshPool = std::make_unique<Pool<MeshComponent>>();
}

//CType RenderView::getSmallestPool() {
//    if (*mMaterialPool <= *mMeshPool) // If material pool is smaller than or equal to mesh pool
//    {
//        if (*mMaterialPool <= *mTransformPool) // If also smaller than transpool
//            return Material;
//        else
//            return Transform;                 // If not smaller than transpool, transpool must be smallest.
//    } else if (*mMeshPool <= *mTransformPool) // If material pool is larger than mesh pool, check if mesh pool is smaller than or equal to transform pool
//    {
//        return Mesh;
//    } else
//        return Transform; // If the two above checks fail then just return the transform pool.
//}

//std::vector<int> RenderView::getViableEntities() {
//    std::vector<int> entityList;
//    mViableEntities.clear();
//    switch (getSmallestPool()) {
//    case Mesh:
//        entityList = mMeshPool->entities();
//        for (auto entity : entityList) {                                                                   // Iterate through the dense list of the smallest pool
//            if (mMaterialPool->has(entityList.at(entity)) && mTransformPool->has(entityList.at(entity))) { // If they exist in the other pools, add that entityID to the list of viable entities
//                mViableEntities.push_back(entity);
//            }
//        }
//        break;
//    case Material:
//        entityList = mMaterialPool->entities();
//        for (auto entity : entityList) {
//            if (mMeshPool->has(entityList.at(entity)) && mTransformPool->has(entityList.at(entity))) {
//                mViableEntities.push_back(entity);
//            }
//        }
//        break;
//    case Transform:
//        entityList = mTransformPool->entities();
//        for (auto entity : entityList) {
//            if (mMaterialPool->has(entityList.at(entity)) && mMeshPool->has(entityList.at(entity))) {
//                mViableEntities.push_back(entity);
//            }
//        }
//        break;
//    default:
//        break;
//    }
//    return mViableEntities;
//}

/**
 * @brief Update RenderView with the new entity
 * @param entityID
 * @return
 */
//void RenderView::addEntity(int entityID) {
//    mViableEntities.emplace_back(entityID);
//    emit updateSystem(std::make_tuple(entityID, mTransformPool->get(entityID), mMaterialPool->get(entityID), mMeshPool->get(entityID)));
//}

//Pool<MeshComponent> *RenderView::getMeshPool() const {
//    return mMeshPool.get();
//}
//Pool<MaterialComponent> *RenderView::getMaterialPool() const {
//    return mMaterialPool.get();
//}
//std::tuple<std::vector<int>, std::vector<TransformComponent *>, std::vector<MaterialComponent *>, std::vector<MeshComponent *>> RenderView::getComponents() {
//    std::vector<MeshComponent *> meshes;
//    std::vector<MaterialComponent *> mats;
//    std::vector<TransformComponent *> transforms;
//    for (auto entity : getViableEntities()) {
//        transforms.emplace_back(mTransformPool->get(entity));
//        mats.emplace_back(mMaterialPool->get(entity));
//        meshes.emplace_back(mMeshPool->get(entity));
//    }
//    return std::make_tuple(mViableEntities, transforms, mats, meshes);
//}
