#ifndef POOL_H
#define POOL_H

#include "components.h"
#include "core.h"
#include "sparseset.h"
#include <QObject>
#include <memory>
#include <vector>
using namespace cjk;
class IPool {
public:
    class iterator;
    virtual ~IPool() = default;
    virtual void remove(int removedEntity) = 0;
    virtual IPool *clone() = 0;
    virtual void cloneComponent(GLuint cloneFrom, GLuint cloneTo) = 0;
    virtual int find(uint eID) const = 0;
    virtual bool has(uint eID) const = 0;
    //    virtual bool has(const Entity &entity) const = 0;
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
};

template <typename Type>
class Pool : public IPool {
public:
    Pool() = default;
    Pool(const Pool &other) {
        mEntities = other.mEntities;
        mComponents = other.mComponents;
    }
    Pool(IPool *copyFrom) {
        auto temp = static_cast<Pool<Type> *>(copyFrom);
        mEntities = temp->mEntities;
        mComponents = temp->mComponents;
    }
    ~Pool() {}
    virtual IPool *clone() override {
        return new Pool<Type>{*this};
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
        mEntities.insert(entityID);

        mComponents.push_back(Type{args...});
    }
    /**
     * @brief cloneComponent creates a new component with the exact same parameters as the old component.
     * @param cloneFrom Entity being cloned
     * @param cloneTo Entity receiving the new component
     */
    void cloneComponent(GLuint cloneFrom, GLuint cloneTo) override {
        assert(!has(cloneTo));
        assert(has(cloneFrom));
        mEntities.insert(cloneTo);
        mComponents.push_back(get(cloneFrom));
    }
    /**
     * @brief Removes an entity by swapping the entityID/component with the last element of the dense arrays and popping out the last element.
     * mEntities.mIndices[removedEntityID] now holds an invalid value for the dense arrays.
     * mEntities.mIndices[swappedEntity] now holds the swapped location of the entityID/component.
     * @param removedEntityID
     */
    inline void remove(int removedEntityID) {
        if (has(removedEntityID)) {
            GLuint swappedEntity{mEntities.back()};
            copy(swappedEntity, removedEntityID); // Swap the removed with the last, then pop out the last.

            mComponents.pop_back();
            mEntities.remove(removedEntityID);
        }
    }
    inline void copy(GLuint eID, GLuint other) {
        assert(has(eID));
        assert(has(other));
        std::swap(mComponents[mEntities.find(eID)], mComponents[mEntities.find(eID)]); // Swap the components to keep the dense set up to date
        mEntities.swap(eID, other);
    }
    /**
     * @brief sort the pool according to a different index
     * @param otherIndex
     */
    inline void sort(std::vector<int> otherIndex) {
        for (size_t i{0}; i < mEntities.size(); i++) {
            if (has(i)) {
                copy(mEntities.get(i), mEntities.get(otherIndex[i]));

            } /*else
                mGroupEnd--;*/
        }
    }
    /**
     * @brief Direct access to the component list
     * @return std::vector containing the component type. Use operator[] to access a component if you know the component exists (isn't out of range) --
     * it's faster than .at() BUT doesn't give an out_of_range exception like .at() does, so it can be harder to debug
     */
    inline std::vector<Type> &data() { return mComponents; }
    /**
     * @brief Returns a list of every entity that exists in the component array.
     * The index is initially meaningless except as an accessor, the component pool can be sorted
     * to keep entities relevant to a certain system first in the array.
     * @return The entity list contains a list of every entityID.
     */
    inline const std::vector<GLuint> &entities() { return mEntities.getList(); }
    /**
     * @brief Returns the sparse array containing an int "pointer" to the mEntityList and mComponentList arrays.
     * The index location is equal to the entityID.
     * @example The location of Entity 5 in the packed arrays can be found at mEntities.mIndices[5].
     * Both IDs and arrays begin at 0.
     * @return
     */
    inline const std::vector<int> &indices() { return mEntities.getIndices(); }
    /**
     * @brief get the component belonging to entity with given ID.
     * Some minor additional overhead due to going through mEntities.mIndices first -
     * if you know you want to use all the entities in the Pool you may want data() instead
     * @param eID
     * @return
     */
    inline Type &get(int eID) {
        assert(has(eID));
        return mComponents[mEntities.getIndex(eID)];
    }
    /**
     * @brief back get the last component in the pool, aka the latest creation
     * @return
     */
    inline Type &back() {
        return mComponents.back();
    }
    inline iterator begin() const {
        const int32_t pos{static_cast<int32_t>(mEntities.size())};
        return iterator{mEntities.list(), pos};
    }
    inline iterator end() const {
        return iterator{mEntities.list(), {}};
    }
    /**
     * @brief find an entity in the index
     * @param eID
     * @return returns -1 (an invalid value) if entity doesn't own a component in the pool
     */
    inline int find(GLuint eID) const override {
        return mEntities.find(eID);
    }
    /**
     * @brief For checking if the pool contains a given entity.
     * @param eID
     * @return
     */
    inline bool has(GLuint eID) const override {
        return find(eID) != -1;
    }
    /**
     * @brief has - alternate function taking Entity reference instead of entityID
     * @param entity actual entity object
     * @return
     */
    //    inline bool has(const Entity &entity) const override {
    //        return find(entity.id()) != -1;
    //    }

    /**
     * @brief Actual number of entities with owned components in the pool.
     * @return
     */
    inline size_t size() const override { return mEntities.size(); }
    inline bool empty() const override { return mEntities.empty(); }
    /**
     * @brief Size of the sparse array.
     * Usually equal to the ID of the latest entity created that owns a component in this pool.
     * @return
     */
    inline size_t extent() { return mEntities.extent(); }
    /**
     * @brief Reset the arrays to empty.
     */
    inline void clear() {
        mEntities.clear();
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
    // Dense Arrays --  n points to Entity in list[n] and component data in mComponentList[n]
    // Both should be the same length.
    std::vector<Type> mComponents;
    SparseSet mEntities;
};

#endif // POOL_H
