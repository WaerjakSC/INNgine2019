#ifndef SPARSESET_H
#define SPARSESET_H
#include "gltypes.h"
#include <vector>
/**
 * The SparseSet class is a custom implementation of the concept commonly called Sparse (or Dense) Set.
 */
class SparseSet {
public:
    /**
     * Pads the mIndex vector to make room for an entityID larger than the current size of the index.
     * @param entityID - ID of the entity to be added.
     */
    void pad(GLuint entityID)
    {
        for (size_t i{mIndex.size()}; i < (size_t)entityID; i++) {
            mIndex.push_back(-1);
        }
    }
    /**
     * Find an entity's position in the sparse set's list
     * @param eID
     * @return returns -1 (an invalid value) if entity doesn't own a component in the pool
     */
    int find(const GLuint eID) const
    {
        if (eID < mIndex.size())
            return mIndex[eID];
        return -1;
    }
    /**
     * Check if an entity is contained within the sparse set.
     * @param eID
     * @return
     */
    bool contains(const GLuint eID) const
    {
        if (find(eID) != -1)
            return true;
        return false;
    }
    /**
     * Actual number of entities with owned components in the pool.
     * @return
     */
    size_t size() const { return mList.size(); }
    bool empty() const { return mList.empty(); }
    /**
     * Size of the sparse array.
     * Usually equal to the ID of the latest entity created that owns a component in this pool.
     * @return
     */
    size_t extent() { return mIndex.size(); }
    /**
     * Remove an entity from the Sparse Set.
     * @param removedEntityID
     * @param noPool If used as a pure sparse set, this should be true. Will cause the Sparse Set to swap before removal.
     */
    void remove(int removedEntityID, bool noPool = true)
    {
        if (noPool) {
            swap(back(), removedEntityID); // Swap the removed with the last, then pop out the last.
        }
        mList.pop_back();
        mIndex[removedEntityID] = -1; // Set entity location to an invalid value.
        if (mList.empty())
            mList.clear();
    }
    /**
     * Reset the arrays to empty.
     */
    void clear()
    {
        mIndex.clear();
        mList.clear();
    }
    /**
     * Returns a copy of the entity at the index specified.
     * @param index
     * @return
     */
    GLuint get(GLuint entityID) const
    {
        return mList[find(entityID)];
    }
    /**
     * Returns a read-write reference to the entityID at the index specified.
     * @param index
     * @return
     */
    GLuint &get(GLuint entityID)
    {
        return mList[index(entityID)];
    }
    /**
     * Returns the index location to mList for the given entityID.
     * @param entityID
     * @return
     */
    const int &index(GLuint entityID) const
    {
        return mIndex[entityID];
    }
    /**
     * Returns a read-write reference to the last element in mList.
     * @return
     */
    GLuint &back()
    {
        return mList.back();
    }
    /**
     * Swaps two entities in the dense set.
     * @param eID
     * @param other
     */
    void swap(GLuint eID, GLuint other)
    {
        std::swap(mList[mIndex[eID]], mList[mIndex[other]]); // Swap the two entities in the pool
        std::swap(mIndex[eID], mIndex[other]);               // Set the index to point to the location after swap
    }
    /**
     * Inserts an entity into the sparse set.
     * Pads the index if entityID value is larger than size of mIndex.
     * @param entityID
     */
    void insert(GLuint entityID)
    {
        if ((size_t)entityID > extent()) {
            pad(entityID);
        }
        if (entityID < extent())
            mIndex[entityID] = size();
        else
            mIndex.push_back(size()); // entity list size is location of new entityID
        mList.push_back(entityID);
    }
    /**
     * Returns a copy of the mIndex vector.
     * @return
     */
    std::vector<int> getIndices() const
    {
        return mIndex;
    }
    /**
     * Returns a copy of the mList vector.
     * @return
     */
    std::vector<GLuint> getList() const
    {
        return mList;
    }
    /**
     * Pointer to the first item in the mList vector.
     * @return
     */
    const GLuint *entities() const
    {
        return mList.data();
    }
    /**
     * Pointer to the mList vector itself. Used by Pool's iterator.
     * @return
     */
    const std::vector<GLuint> *listPtr() const
    {
        return &mList;
    }

private:
    /// Sparse array -- index is the entityID. Value contained is the index location of each entityID in mEntityList
    std::vector<int> mIndex;
    /// Contains the ID of each entity.
    std::vector<GLuint> mList;
};

#endif // SPARSESET_H
