#ifndef POOL_H
#define POOL_H

#include "core.h"
#include "sparseset.h"
#include <vector>
/**
 * The IPool class is the base Interface class for the Pool<Type> class.
 */
class IPool {
public:
    class iterator;
    virtual ~IPool() = default;
    virtual void remove(int removedEntity) = 0;
    virtual IPool *clone() = 0;
    virtual void cloneComponent(GLuint cloneFrom, GLuint cloneTo) = 0;
    virtual void swap(const GLuint lhs, const GLuint rhs) = 0;
    virtual int find(const GLuint eID) const = 0;
    virtual bool has(const GLuint eID) const = 0;
    virtual const GLuint *entities() const = 0;
    virtual const int &index(GLuint entityID) const = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual iterator begin() const = 0;
    virtual iterator end() const = 0;
    /**
     * Custom iterator class to let us use range-based for loops and such.
     */
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

        iterator &operator++()
        {
            return --index, *this;
        }

        iterator operator++(int)
        {
            iterator orig = *this;
            return ++(*this), orig;
        }

        iterator &operator--()
        {
            return ++index, *this;
        }

        iterator operator--(int)
        {
            iterator orig = *this;
            return --(*this), orig;
        }

        iterator &operator+=(const difference_type value)
        {
            index -= value;
            return *this;
        }

        iterator operator+(const difference_type value) const
        {
            return iterator{direct, index - value};
        }

        iterator &operator-=(const difference_type value)
        {
            return (*this += -value);
        }

        iterator operator-(const difference_type value) const
        {
            return (*this + -value);
        }

        difference_type operator-(const iterator &other) const
        {
            return other.index - index;
        }

        reference operator[](const difference_type value) const
        {
            const auto pos = size_t(index - value - 1);
            return (*direct)[pos];
        }

        bool operator==(const iterator &other) const
        {
            return other.index == index;
        }

        bool operator!=(const iterator &other) const
        {
            return !(*this == other);
        }

        bool operator<(const iterator &other) const
        {
            return index > other.index;
        }

        bool operator>(const iterator &other) const
        {
            return index < other.index;
        }

        bool operator<=(const iterator &other) const
        {
            return !(*this > other);
        }

        bool operator>=(const iterator &other) const
        {
            return !(*this < other);
        }

        pointer operator->() const
        {
            const auto pos = size_t(index - 1);
            return &(*direct)[pos];
        }

        reference operator*() const
        {
            return *operator->();
        }

    private:
        const std::vector<GLuint> *direct;
        std::int32_t index;
    };
};

template <typename Type>
/**
 * The Pool class is a wrapper containing a Sparse Set of entities (GLuint) and their components.
 * The Sparse Set serves as an index for each entity's component for quick look-up with minimal cache misses.
 */
class Pool : public IPool {
public:
    Pool() = default;
    Pool(const Pool &other)
    {
        mEntities = other.mEntities;
        mComponents = other.mComponents;
    }
    Pool(IPool *copyFrom)
    {
        auto temp = static_cast<Pool<Type> *>(copyFrom);
        mEntities = temp->mEntities;
        mComponents = temp->mComponents;
    }
    ~Pool() {}
    virtual IPool *clone() override
    {
        return new Pool<Type>{*this};
    }

    /**
     * Adds an entity and its new component to this pool.
     * Entity is equivalent to Component in this case, since pool won't contain the entity if the entity doesn't have the component.
     * @example the Transform component type can take 3 extra variables, add<Transform>(entityID, position, rotation, scale) will initialize its variables to custom values.
     * @tparam Args... args Variadic parameter pack - Use as many function parameters as needed to construct the component.
     * @param entityID
     */
    template <class... Args>
    Type &add(GLuint entityID, Args... args)
    {
        assert(!has(entityID)); // Make sure the entityID is unique.
        mEntities.insert(entityID);
        mComponents.push_back(Type{args...});
        return mComponents.back();
    }
    /**
     * Creates a new component with the exact same parameters as the old component.
     * @param cloneFrom Entity being cloned
     * @param cloneTo Entity receiving the new component
     */
    void cloneComponent(GLuint cloneFrom, GLuint cloneTo) override
    {
        assert(has(cloneFrom));
        mEntities.insert(cloneTo);
        mComponents.push_back(get(cloneFrom));
    }
    /**
     * Removes an entity by swapping the entityID/component with the last element of the dense arrays and popping out the last element.
     * mEntities.mIndices[removedEntityID] now holds an invalid value for the dense arrays.
     * mEntities.mIndices[swappedEntity] now holds the swapped location of the entityID/component.
     * @param removedEntityID
     */
    void remove(int removedEntityID)
    {
        if (has(removedEntityID)) {
            GLuint swappedEntity{mEntities.back()};
            swap(swappedEntity, removedEntityID); // Swap the removed with the last, then pop out the last.
            mComponents.pop_back();
            mEntities.remove(removedEntityID, false);
        }
    }
    /**
     * Swaps one Entity's position in the Pool with another.
     * Most commonly used when removing an entity - swaps the entity being removed with the last entity in the list and pops it out.
     * @param entityID
     * @param otherEntityID
     */
    void swap(GLuint entityID, GLuint otherEntityID)
    {
        assert(has(entityID));
        assert(has(otherEntityID));
        std::swap(mComponents[mEntities.find(entityID)], mComponents[mEntities.find(otherEntityID)]); // Swap the components to keep the dense set up to date
        mEntities.swap(entityID, otherEntityID);
    }

    /**
     * Sort the pool according to a different index.
     * @param otherIndex
     */
    void sort(std::vector<int> otherIndex)
    {
        for (size_t i{0}; i < mEntities.size(); i++) {
            if (has(i)) {
                swap(mEntities.get(i), mEntities.get(otherIndex[i]));

            } /*else
                mGroupEnd--;*/
        }
    }
    /**
     * Direct access to the component list.
     * Use operator[] to access a component if you know the component exists (isn't out of range) --
     * it's faster than .at() BUT doesn't give an out_of_range exception like .at() does, so it can be harder to debug.
     * @return std::vector containing the component type.
     */
    std::vector<Type> &data() { return mComponents; }
    /**
     * @brief Calls mComponents.data().
     * raw() + size() will return the last member of the vector.
     * @return
     */
    Type *raw() { return mComponents.data(); }
    /**
     * Returns a list of every entity that exists in the component array.
     * The index is initially meaningless except as an accessor, the component pool can be sorted
     * to keep entities relevant to a certain system first in the array.
     * @return The entity list contains a list of every entityID.
     */
    const std::vector<GLuint> entityList() { return mEntities.getList(); }

    /**
     * Pointer to the first entity in the sparse set list. Guaranteed to be tightly packed.
     * @return
     */
    const GLuint *entities() const override { return mEntities.entities(); }

    /**
     * Returns the sparse array containing an int "pointer" to the mEntityList and mComponentList arrays.
     * The index location is equal to the entityID.
     * @example The location of Entity 5 in the packed arrays can be found at mEntities.mIndices[5].
     * Both IDs and arrays begin at 0.
     * @return
     */
    const std::vector<int> &indices() { return mEntities.getIndices(); }
    /**
     * Find the location of the entityID in the Sparse Set's entity list.
     * Can return -1, in which case the entity is not contained within.
     * @param entityID
     * @return
     */
    const int &index(GLuint entityID) const
    {
        return mEntities.index(entityID);
    }
    /**
     * Get the component belonging to entity with given ID.
     * Some minor additional overhead due to going through mEntities.mIndices first -
     * if you know you want to use all the entities in the Pool you may want data() instead.
     * @param eID
     * @return
     */
    Type &get(int eID)
    {
        assert(has(eID));
        return mComponents[mEntities.index(eID)];
    }
    /**
     * Back get the last component in the pool, aka the latest creation.
     * @return
     */
    Type &back()
    {
        return mComponents.back();
    }
    iterator begin() const
    {
        const int32_t pos{static_cast<int32_t>(mEntities.size())};
        return iterator{mEntities.listPtr(), pos};
    }
    iterator end() const
    {
        return iterator{mEntities.listPtr(), {}};
    }
    /**
     * Find an entity's position in the sparse set.
     * @param eID
     * @return returns -1 (an invalid value) if entity doesn't own a component in the pool
     */
    int find(const GLuint eID) const override
    {
        return mEntities.find(eID);
    }
    /**
     * For checking if the pool contains a given entity.
     * @param eID
     * @return
     */
    bool has(GLuint eID) const override
    {
        return find(eID) != -1;
    }

    /**
     * Actual number of entities with owned components in the pool.
     * @return
     */
    size_t size() const override { return mEntities.size(); }
    /**
     * Check if the Pool is empty.
     * @return
     */
    bool empty() const override { return mEntities.empty(); }
    /**
     * Size of the sparse array.
     * Usually equal to the ID of the latest entity created that owns a component in this pool.
     * @return
     */
    size_t extent() { return mEntities.extent(); }
    /**
     * Reset the arrays to empty.
     */
    void clear()
    {
        mEntities.clear();
        mComponents.clear();
    }
    // Comparison operator overloads
    template <typename Type2>
    bool operator>(Pool<Type2> &other)
    {
        return size() > other.size();
    }
    template <typename Type2>
    bool operator<(Pool<Type2> &other)
    {
        return size() < other.size();
    }
    template <typename Type2>
    bool operator>=(Pool<Type2> &other)
    {
        return size() >= other.size();
    }
    template <typename Type2>
    bool operator<=(Pool<Type2> &other)
    {
        return size() <= other.size();
    }
    void onDestroy(int removedEntity);
    void onConstruct(GLuint entityID);

private:
    /// Sparse Set --  n points to Entity in list[n] and component data in mComponentList[n].
    SparseSet mEntities;
    std::vector<Type> mComponents;
};

#endif // POOL_H
