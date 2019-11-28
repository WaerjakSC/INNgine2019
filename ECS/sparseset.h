#ifndef SPARSESET_H
#define SPARSESET_H
#include "gsl_math.h"
class SparseSet {
public:
    /**
     * @brief pads the mIndex vector to make room for an entityID larger than the current size of the index.
     * @param entityID - ID of the entity to be added.
     */
    void padIndex(GLuint entityID) {
        for (size_t i{mIndex.size()}; i < (size_t)entityID; i++) {
            mIndex.push_back(-1);
        }
    }
    /**
     * @brief find an entity in the index
     * @param eID
     * @return returns -1 (an invalid value) if entity doesn't own a component in the pool
     */
    inline int find(GLuint eID) const {
        if (eID < mIndex.size())
            return mIndex[eID];
        return -1;
    }
    /**
     * @brief Actual number of entities with owned components in the pool.
     * @return
     */
    inline size_t size() const { return mList.size(); }
    inline bool empty() const { return mList.empty(); }
    /**
     * @brief Size of the sparse array.
     * Usually equal to the ID of the latest entity created that owns a component in this pool.
     * @return
     */
    inline size_t extent() { return mIndex.size(); }
    inline void remove(int removedEntityID) {
        mList.pop_back();
        mIndex[removedEntityID] = -1; // Set entity location to an invalid value.
        if (mList.empty())
            mList.clear();
    }
    /**
     * @brief Reset the arrays to empty.
     */
    inline void clear() {
        mIndex.clear();
        mList.clear();
    }
    /**
     * @brief Returns a copy of the entity at the index specified.
     * @param index
     * @return
     */
    inline GLuint get(GLuint entityID) const {
        return mList[find(entityID)];
    }
    /**
     * @brief Returns a read-write reference to the entityID at the index specified.
     * @param index
     * @return
     */
    inline GLuint &get(GLuint entityID) {
        return mList[getIndex(entityID)];
    }
    /**
     * @brief getIndex returns the index location to mList for the given entityID
     * @param entityID
     * @return
     */
    inline int &getIndex(GLuint entityID) {
        return mIndex[entityID];
    }
    /**
     * @brief back returns a read-write reference to the last element in mList.
     * @return
     */
    inline GLuint &back() {
        return mList.back();
    }
    void swap(int eID, int other) {
        std::swap(mList[mIndex[eID]], mList[mIndex[other]]); // Swap the two entities in the pool
        std::swap(mIndex[eID], mIndex[other]);               // Set the index to point to the location after swap
    }
    /**
     * @brief insert Inserts an entity into the sparse set.
     * @param entityID
     */
    void insert(GLuint entityID) {
        if ((size_t)entityID > extent()) {
            padIndex(entityID);
        }
        if (entityID < extent())
            mIndex[entityID] = size();
        else
            mIndex.push_back(size()); // entity list size is location of new entityID
        mList.push_back(entityID);
    }

    std::vector<int> getIndices() const {
        return mIndex;
    }

    const std::vector<GLuint> &getList() const {
        return mList;
    }
    const std::vector<GLuint> *list() const {
        return &mList;
    }

private:
    std::vector<int> mIndex;   // Sparse array -- index is the entityID. Value contained is the index location of each entityID in mEntityList
    std::vector<GLuint> mList; // Value is entityID.
};

#endif // SPARSESET_H
