#ifndef POOL_H
#define POOL_H
#include "Components/comppch.h"
#include <vector>
template <typename Type>
class Pool {
public:
    Pool() {}
    ~Pool() {
        for (auto components : mComponentList)
            delete components;
    }
    /**
     * @brief Adds an entity and its new component to this pool.
     * Entity is equivalent to Component in this case, since pool won't contain the entity if the entity doesn't have the component.
     * @param entityID
     */
    void add(int entityID) {
        assert(!has(entityID)); // Make sure the entityID is unique.
        if ((size_t)entityID > mEntityIndices.size()) {
            for (size_t i = mEntityIndices.size(); i < (size_t)entityID; i++) {
                mEntityIndices.push_back(-1);
            }
        }
        mEntityIndices.push_back(mEntityList.size()); // entity list size is location of new entityID
        mEntityList.push_back(entityID);
        mComponentList.push_back(new Type());
    }
    /**
     * @brief Removes an entity by swapping swapping the entityID/component with the last element of the dense arrays and popping out the last element.
     * mEntityIndices[removedEntityID] now holds an impossible value for the dense arrays.
     * mEntityIndices[swappedEntity] now holds the swapped location of the entityID/component.
     * @param removedEntityID
     */
    void remove(int removedEntityID) {
        assert(has(removedEntityID));                                                   // Make sure the entity you want to delete actually exists. For debugging purposes.
        mEntityIndices.at(mEntityList.back()) = mEntityIndices.at(removedEntityID);     // Set the index to point to the location after swap
        std::swap(mEntityList.at(mEntityIndices[removedEntityID]), mEntityList.back()); // Swap the removed with the last, then pop out the last.
        mEntityList.pop_back();
        std::swap(mComponentList.at(mEntityIndices[removedEntityID]), mComponentList.back());
        mComponentList.pop_back();
        mEntityIndices.at(removedEntityID) = -1; // Set entity location to an invalid value.
    }
    std::vector<Type *> getComponents() { return mComponentList; } // Direct access to the components
    const std::vector<int> getEntityList() { return mEntityList; }
    const std::vector<int> getEntityIndices() { return mEntityIndices; }
    /**
     * @brief get the component belonging to entity with given ID.
     * @param eID
     * @return
     */
    Type *get(int eID) {
        assert(has(eID));
        if (has(eID)) {
            return mComponentList.at(mEntityIndices.at(eID));
        } else {
            qDebug() << "Entity not found!";
            return mComponentList.at(mEntityIndices.back());
        }
    }
    /**
     * @brief get the last component in the pool, aka the latest creation
     * @return
     */
    Type *getLast() {
        return mComponentList.back();
    }

    /**
     * @brief For checking if the pool contains a given entity.
     * Utility function for View.
     * @param eID
     * @return
     */
    bool has(GLuint eID) {
        if (mEntityIndices.size() > eID)
            return mEntityIndices.at(eID) != -1;
        return false;
    }
    /**
     * @brief size
     * @return Size of the dense arrays.
     */
    size_t size() { return mEntityList.size(); }
    /**
     * @brief Reset the arrays to empty.
     */
    void clear() {
        mEntityIndices.clear();
        mEntityList.clear();
        mComponentList.clear();
    }

private:
    std::vector<int> mEntityIndices; // Sparse array -- index is the entityID. Value contained is the index location of each entityID in mEntityList

    // Dense Arrays --  n points to Entity in mEntityList[n] and component data in mComponentList[n]
    // Both should be the same length.
    std::vector<int> mEntityList; // Value is entityID.
    std::vector<Type *> mComponentList;
};

#endif // POOL_H
