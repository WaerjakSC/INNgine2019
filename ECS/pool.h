#ifndef POOL_H
#define POOL_H

#include "components.h"
#include "core.h"
#include "entity.h"
#include <QObject>
#include <memory>
#include <vector>
using namespace cjk;
class IPool {
public:
    class iterator;
    virtual ~IPool() = default;
    virtual void remove(int removedEntity) = 0;
    virtual Ref<IPool> clone() = 0;
    virtual void cloneComponent(GLuint cloneFrom, GLuint cloneTo) = 0;
    virtual void swap(Ref<IPool> other) = 0;
    virtual int find(uint eID) const = 0;
    virtual bool has(uint eID) const = 0;
    virtual bool has(const Entity &entity) const = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual iterator begin() const = 0;
    virtual iterator end() const = 0;

    class iterator {
        friend class IPool;

    public:
        iterator(const std::vector<GLuint> *ref, const std::int32_t idx)
            : direct{ref}, index{idx} {}
        using difference_type = std::int32_t;
        using pointer = const GLuint *;
        using reference = const GLuint &;
        using iterator_category = std::random_access_iterator_tag;

        iterator() = default;

        iterator &operator++() {
            return --index, *this;
        }

        iterator operator++(int) {
            iterator orig = *this;
            return ++(*this), orig;
        }

        iterator &operator--() {
            return ++index, *this;
        }

        iterator operator--(int) {
            iterator orig = *this;
            return --(*this), orig;
        }

        iterator &operator+=(const difference_type value) {
            index -= value;
            return *this;
        }

        iterator operator+(const difference_type value) const {
            return iterator{direct, index - value};
        }

        iterator &operator-=(const difference_type value) {
            return (*this += -value);
        }

        iterator operator-(const difference_type value) const {
            return (*this + -value);
        }

        difference_type operator-(const iterator &other) const {
            return other.index - index;
        }

        reference operator[](const difference_type value) const {
            const auto pos = size_t(index - value - 1);
            return (*direct)[pos];
        }

        bool operator==(const iterator &other) const {
            return other.index == index;
        }

        bool operator!=(const iterator &other) const {
            return !(*this == other);
        }

        bool operator<(const iterator &other) const {
            return index > other.index;
        }

        bool operator>(const iterator &other) const {
            return index < other.index;
        }

        bool operator<=(const iterator &other) const {
            return !(*this > other);
        }

        bool operator>=(const iterator &other) const {
            return !(*this < other);
        }

        pointer operator->() const {
            const auto pos = size_t(index - 1);
            return &(*direct)[pos];
        }

        reference operator*() const {
            return *operator->();
        }

    private:
        const std::vector<GLuint> *direct;
        std::int32_t index;
    };

protected:
    std::vector<int> mIndex;   // Sparse array -- index is the entityID. Value contained is the index location of each entityID in mEntityList
    std::vector<GLuint> mList; // Value is entityID.
};

template <typename Type>
class Pool : public IPool {
public:
    Pool() = default;
    Pool(Pool *other) {
        mList = other->mList;
        mIndex = other->mIndex;
        mComponents = other->mComponents;
    }
    ~Pool() {}
    virtual Ref<IPool> clone() override {
        return std::make_shared<Pool>(*this);
    }
    virtual void swap(Ref<IPool> other) override {
        auto swapped = std::static_pointer_cast<Pool<Type>>(other);
        mList = swapped->mList;
        mIndex = swapped->mIndex;
        mComponents = swapped->mComponents;
    }

    /**
     * @brief Adds an entity and its new component to this pool.
     * Entity is equivalent to Component in this case, since pool won't contain the entity if the entity doesn't have the component.
     * @example the Transform component type can take 3 extra variables, add<Transform>(entityID, position, rotation, scale) will initialize its variables to custom values.
     * @tparam Args... args Variadic parameter pack - Use as many function parameters as needed to construct the component.
     * @param entityID
     */
    template <class... Args>
    void add(GLuint entityID, Args... args) {
        assert(!has(entityID)); // Make sure the entityID is unique.
        if ((size_t)entityID > mIndex.size()) {
            for (size_t i = mIndex.size(); i < (size_t)entityID; i++) {
                mIndex.push_back(-1);
            }
        }
        if (entityID < mIndex.size())
            mIndex[entityID] = mList.size();
        else
            mIndex.push_back(mList.size()); // entity list size is location of new entityID
        mList.push_back(entityID);
        mComponents.push_back(Type(args...));
    }
    /**
     * @brief cloneComponent creates a new component with the exact same parameters as the old component.
     * @param cloneFrom Entity being cloned
     * @param cloneTo Entity receiving the new component
     */
    void cloneComponent(GLuint cloneFrom, GLuint cloneTo) override {
        assert(!has(cloneTo));
        assert(has(cloneFrom));
        if ((size_t)cloneTo > mIndex.size()) {
            for (size_t i = mIndex.size(); i < (size_t)cloneTo; i++) {
                mIndex.push_back(-1);
            }
        }
        if (cloneTo < mIndex.size())
            mIndex[cloneTo] = mList.size();
        else
            mIndex.push_back(mList.size()); // entity list size is location of new entityID        mList.push_back(cloneTo);
        Type component(get(cloneFrom));
        mList.push_back(cloneTo);
        mComponents.push_back(component);
    }
    /**
     * @brief Removes an entity by swapping the entityID/component with the last element of the dense arrays and popping out the last element.
     * mEntities.mIndices[removedEntityID] now holds an invalid value for the dense arrays.
     * mEntities.mIndices[swappedEntity] now holds the swapped location of the entityID/component.
     * @param removedEntityID
     */
    void remove(int removedEntityID) {
        if (has(removedEntityID)) {
            GLuint swappedEntity = mList.back();
            swap(swappedEntity, removedEntityID); // Swap the removed with the last, then pop out the last.
            mList.pop_back();
            mComponents.pop_back();
            mIndex[removedEntityID] = -1; // Set entity location to an invalid value.
        }
        if (mList.empty())
            mIndex.clear();
    }
    void swap(GLuint eID, GLuint other) {
        assert(has(eID));
        assert(has(other));
        std::swap(mList[mIndex[eID]], mList[mIndex[other]]);             // Swap the two entities in the pool
        std::swap(mComponents[mIndex[eID]], mComponents[mIndex[other]]); // Swap the components to keep the dense set up to date
        std::swap(mIndex[eID], mIndex[other]);                           // Set the index to point to the location after swap
    }
    /**
     * @brief sort the pool according to a different index
     * @param otherIndex
     */
    void sort(std::vector<int> otherIndex) {
        for (size_t i = 0; i < mList.size(); i++) {
            if (has(i)) {
                swap(mList[mIndex[i]], mList[otherIndex[i]]);

            } /*else
                mGroupEnd--;*/
        }
    }
    /**
     * @brief Direct access to the component list
     * @return std::vector containing the component type. Use operator[] to access a component if you know the component exists (isn't out of range) --
     * it's faster than .at() BUT doesn't give an out_of_range exception like .at() does, so it can be harder to debug
     */
    std::vector<Type> &data() { return mComponents; }
    /**
     * @brief Returns a list of every entity that exists in the component array.
     * The index is initially meaningless except as an accessor, the component pool can be sorted
     * to keep entities relevant to a certain system first in the array.
     * @return The entity list contains a list of every entityID.
     */
    const std::vector<GLuint> &entities() { return mList; }
    /**
     * @brief Returns the sparse array containing an int "pointer" to the mEntityList and mComponentList arrays.
     * The index location is equal to the entityID.
     * @example The location of Entity 5 in the packed arrays can be found at mEntities.mIndices[5].
     * Both IDs and arrays begin at 0.
     * @return
     */
    const std::vector<int> &indices() { return mIndex; }
    /**
     * @brief get the component belonging to entity with given ID.
     * Some minor additional overhead due to going through mEntities.mIndices first -
     * if you know you want to use all the entities in the Pool you may want data() instead
     * @param eID
     * @return
     */
    Type &get(int eID) {
        assert(has(eID));
        return mComponents[mIndex[eID]];
    }
    /**
     * @brief back get the last component in the pool, aka the latest creation
     * @return
     */
    Type &back() {
        return mComponents.back();
    }
    iterator begin() const {
        const int32_t pos = mList.size();
        return iterator{&mList, pos};
    }
    iterator end() const {
        return iterator{&mList, {}};
    }
    /**
     * @brief find an entity in the index
     * @param eID
     * @return returns -1 (an invalid value) if entity doesn't own a component in the pool
     */
    int find(GLuint eID) const override {
        if (eID < mIndex.size())
            return mIndex[eID];
        return -1;
    }
    /**
     * @brief For checking if the pool contains a given entity.
     * @param eID
     * @return
     */
    bool has(GLuint eID) const override {
        return find(eID) != -1;
    }
    /**
     * @brief has - alternate function taking Entity reference instead of entityID
     * @param entity actual entity object
     * @return
     */
    bool has(const Entity &entity) const override {
        return find(entity.id()) != -1;
    }

    /**
     * @brief Actual number of entities with owned components in the pool.
     * @return
     */
    size_t size() const override { return mList.size(); }
    bool empty() const override { return mList.empty(); }
    /**
     * @brief Size of the sparse array.
     * Usually equal to the ID of the latest entity created that owns a component in this pool.
     * @return
     */
    size_t extent() { return mIndex.size(); }
    /**
     * @brief Reset the arrays to empty.
     */
    void clear() {
        mIndex.clear();
        mList.clear();
        mComponents.clear();
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
    // index vector is in IPool
    // Dense Arrays --  n points to Entity in list[n] and component data in mComponentList[n]
    // Both should be the same length.
    std::vector<Type> mComponents;
};

#endif // POOL_H
