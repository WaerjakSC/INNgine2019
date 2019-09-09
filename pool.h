#ifndef POOL_H
#define POOL_H
#include <vector>
template <typename Type>
class Pool {
public:
    Pool() {}
    /**
     * @brief Adds an entity and its new component to this pool.
     * Entity is equivalent to Component in this case, since pool won't contain the entity if the entity doesn't have the component.
     * @param entityID
     */
    void addEntity(int entityID) {
        mEntityIndices.push_back(mEntityList.size()); // entity list size is location of new entityID
        mEntityList.push_back(entityID);
        mComponentList.push_back(Type());
    }
    /**
     * @brief Removes an entity by swapping swapping the entityID/component with the last element of the dense arrays and popping out the last element.
     * mEntityIndices[removedEntityID] now holds an impossible value for the dense arrays.
     * mEntityIndices[swappedEntity] now holds the swapped location of the entityID/component.
     * @param removedEntityID
     */
    void removeEntity(int removedEntityID) {
        int swappedEntity = mEntityList.back();                                         // Get the ID of the entity being swapped
        mEntityIndices.at(swappedEntity) = mEntityIndices.at(removedEntityID);          // Set the index to point to the location after swap
        std::swap(mEntityList.at(mEntityIndices[removedEntityID]), mEntityList.back()); // Swap the removed with the last, then pop out the last.
        mEntityList.pop_back();
        std::swap(mComponentList.at(mEntityIndices[removedEntityID]), mComponentList.back());
        mComponentList.pop_back();
        mEntityIndices.at(removedEntityID) = -1; // Set entity location to an invalid value.
    }
    const std::vector<Type> &getComponents() { return &mComponentList; } // Direct access to the components
    const std::vector<int> getEntityList() { return mEntityList; }
    const std::vector<int> getEntityIndices() { return mEntityIndices; }

    /**
     * @brief For checking if the pool contains a given entity
     * Utility function for View
     * @param eID
     * @return
     */
    bool hasComponent(int eID) {
        return mEntityIndices.at(eID) != -1;
    }

private:
    std::vector<int> mEntityIndices; // Sparse array -- index is the entityID. Value contained is the index location of each entityID in mEntityList

    // Dense Arrays --  n points to Entity in mEntityList[n] and component data in mComponentList[n]
    // Both should be the same length.
    std::vector<int> mEntityList; // Value is entityID.
    std::vector<Type> mComponentList;
};

#endif // POOL_H
