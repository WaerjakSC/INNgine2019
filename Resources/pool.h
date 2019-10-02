#ifndef POOL_H
#define POOL_H

#include "components.h"
#include <QObject>
#include <vector>
class IPool {
public:
    virtual ~IPool() = default;
    virtual void remove(int removedEntity) = 0;
};

template <typename Type>
class Pool : public IPool {
public:
    Pool() = default;
    ~Pool() {}
    /**
     * @brief Adds an entity and its new component to this pool.
     * Entity is equivalent to Component in this case, since pool won't contain the entity if the entity doesn't have the component.
     * Could maybe have template<...Args> here for component initialization?
     * @example the Transform component type can take 3 extra variables, add<Transform>(entityID, position, rotation, scale) will initialize its variables to custom values.
     * @tparam Args... args Variadic parameter pack - Use as many function parameters as needed to construct the component.
     * @param entityID
     */
    template <class... Args>
    void add(int entityID, Args... args) {
        assert(!has(entityID)); // Make sure the entityID is unique.
        if ((size_t)entityID > mEntityIndices.size()) {
            for (size_t i = mEntityIndices.size(); i < (size_t)entityID; i++) {
                mEntityIndices.push_back(-1);
            }
        }
        mEntityIndices.push_back(mEntityList.size()); // entity list size is location of new entityID
        mEntityList.push_back(entityID);
        mComponentList.push_back(Type(args...));
    }
    /**
     * @brief Removes an entity by swapping swapping the entityID/component with the last element of the dense arrays and popping out the last element.
     * mEntityIndices[removedEntityID] now holds an invalid value for the dense arrays.
     * mEntityIndices[swappedEntity] now holds the swapped location of the entityID/component.
     * @param removedEntityID
     */
    void remove(int removedEntityID) {
        if (has(removedEntityID)) {
            // Make sure the entity you want to delete actually exists. For debugging purposes.
            mEntityIndices[mEntityList.back()] = mEntityIndices[removedEntityID];        // Set the index to point to the location after swap
            std::swap(mEntityList[mEntityIndices[removedEntityID]], mEntityList.back()); // Swap the removed with the last, then pop out the last.
            mEntityList.pop_back();
            std::swap(mComponentList.at(mEntityIndices[removedEntityID]), mComponentList.back());
            mComponentList.pop_back();
            mEntityIndices[removedEntityID] = -1; // Set entity location to an invalid value.
        }
        if (mEntityList.empty())
            mEntityIndices.clear();
    }
    /**
     * @brief Direct access to the component list
     * @return std::vector containing the component type. Use operator[] to access a component if you know the component exists (isn't out of range) --
     * it's faster than .at() BUT doesn't give an out_of_range exception like .at() does, so it can be harder to debug
     */
    std::vector<Type> &data() { return mComponentList; }
    /**
     * @brief Returns a list of every entity that exists in the component array.
     * The index is initially meaningless except as an accessor, the component pool can be sorted
     * to keep entities relevant to a certain system first in the array.
     * @return The entity list contains a list of every entityID.
     */
    const std::vector<int> &entities() { return mEntityList; }
    /**
     * @brief Returns the sparse array containing an int "pointer" to the mEntityList and mComponentList arrays.
     * The index location is equal to the entityID. @example The location of Entity 5 in the packed arrays can be found at mEntityIndices[5].
     * Both IDs and arrays begin at 0.
     * @return
     */
    const std::vector<int> &indices() { return mEntityIndices; }
    /**
     * @brief get the component belonging to entity with given ID.
     * Some minor additional overhead due to going through mEntityIndices first -
     * if you know you want to use all the entities in the Pool you may want data() instead
     * @param eID
     * @return
     */
    Type &get(int eID) {
        assert(has(eID));
        return mComponentList.at(mEntityIndices.at(eID));
    }
    /**
     * @brief get the last component in the pool, aka the latest creation
     * @return
     */
    Type &getLast() {
        return mComponentList.back();
    }

    /**
     * @brief For checking if the pool contains a given entity.
     * @param eID
     * @return
     */
    bool has(GLuint eID) {
        if (mEntityIndices.size() > eID)
            return mEntityIndices.at(eID) != -1;
        return false;
    }
    void swap(GLuint eID, GLuint other) {
        assert(has(eID));
        assert(has(other));
        std::swap(mEntityList[mEntityIndices[eID]], mEntityList[mEntityIndices[other]]);       // Swap the two entities in the pool
        std::swap(mComponentList[mEntityIndices[eID]], mComponentList[mEntityIndices[other]]); // Swap the components to keep the dense set up to date
        std::swap(mEntityIndices[eID], mEntityIndices[other]);                                 // Set the value for mEntityIndices to the new value.
    }
    /**
     * @brief sort the pool according to a different index
     * @param otherIndex
     */
    void sort(std::vector<int> otherIndex) {
        for (size_t i = 0; i < mEntityList.size(); i++) {
            if (has(i)) {
                swap(mEntityList[mEntityIndices[i]], mEntityList[otherIndex[i]]);
            }
        }
    }
    /**
     * @brief Actual number of entities with owned components in the pool.
     * @return
     */
    size_t size() { return mEntityList.size(); }
    /**
     * @brief Size of the sparse array.
     * Usually equal to the ID of the latest entity created that owns a component in this pool.
     * @return
     */
    size_t extent() { return mEntityIndices.size(); }
    /**
     * @brief Reset the arrays to empty.
     */
    void clear() {
        mEntityIndices.clear();
        mEntityList.clear();
        mComponentList.clear();
    }
    // Comparison operator overloads
    template <typename Type2>
    bool operator>(Pool<Type2> &other) {
        return size() > other.size();
    }
    template <typename Type2>
    bool operator<(Pool<Type2> &other) {
        return size() < other.size();
    }
    template <typename Type2>
    bool operator>=(Pool<Type2> &other) {
        return size() >= other.size();
    }
    template <typename Type2>
    bool operator<=(Pool<Type2> &other) {
        return size() <= other.size();
    }

private:
    std::vector<int> mEntityIndices; // Sparse array -- index is the entityID. Value contained is the index location of each entityID in mEntityList

    // Dense Arrays --  n points to Entity in mEntityList[n] and component data in mComponentList[n]
    // Both should be the same length.
    std::vector<int> mEntityList; // Value is entityID.
    std::vector<Type> mComponentList;
};

#endif // POOL_H
