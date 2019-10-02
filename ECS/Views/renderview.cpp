#include "renderview.h"
#include "registry.h"
RenderView::RenderView(/*std::shared_ptr<Pool<Transform>> tf*/) {
    //    mMaterialPool = std::make_unique<Pool<Material>>();
    //    mMeshPool = std::make_unique<Pool<Mesh>>();
}

void RenderView::sortGroup() {
    Registry *registry = Registry::instance();
    for (size_t index = 0; index < mTransformPool->entities().size(); index++) {
        if (!registry->contains(mTransformPool->entities()[index], CType::Transform | CType::Material | CType::Mesh)) { // If it doesn't contain these three types
            for (size_t other = index + 1; other < mTransformPool->entities().size(); other++)                          // Search through the rest of the pool for an entity that does have those types and swap it so that other entity is first
                if (registry->contains(mTransformPool->entities()[other], CType::Transform | CType::Material | CType::Mesh)) {
                    mTransformPool->swap(mTransformPool->entities()[index], mTransformPool->entities()[other]);
                    break;
                }
        }
    }
    mMeshPool->sort(mTransformPool->indices());
    mMaterialPool->sort(mTransformPool->indices());
    lastRenderItem = mMeshPool->size() - 1;
    // After this, should be able to iterate through sequentially in all three pools without needing to check if the entity exists.
}
