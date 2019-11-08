#include "registry.h"
#include "billboard.h"
#include <optional>
Registry *Registry::mInstance = nullptr;

Registry::Registry() {
}
Registry *Registry::instance() {
    if (!mInstance)
        mInstance = new Registry();
    return mInstance;
}

void Registry::removeBillBoardID(GLuint entityID) {
    for (auto &billboard : mBillBoards) {
        if (entityID == billboard) {
            std::swap(billboard, mBillBoards.back());
            mBillBoards.pop_back();
            return;
        }
    }
}

bool Registry::isBillBoard(GLuint entityID) {
    for (auto billboard : mBillBoards) {
        if (entityID == billboard) {
            return true;
        }
    }
    return false;
}
/**
 * @brief Get a pointer to the entity with the specified ID.
 * @param eID
 * @return
 */
Ref<Entity> Registry::getEntity(GLuint eID) {
    auto search = mEntities.find(eID);
    if (search != mEntities.end())
        return search->second;
    return nullptr;
}

/**
 * @brief Destroy gameobject
 * @param eID - entityID
 */
void Registry::removeEntity(GLuint eID) {
    if (isBillBoard(eID)) {
        removeBillBoardID(eID);
    }
    if (contains<Transform>(eID)) {
        setParent(eID, -1);
    }
    entityDestroyed(eID);                   // Pass the message on to the registry
    mEntities.find(eID)->second->destroy(); // doesn't actually destroy the entity object, simply clears the data and increments the generation variable

    emit entityRemoved(eID);
}

GLuint Registry::nextAvailable() {
    for (auto entity : mEntities)
        if (entity.second->isDestroyed())
            return entity.first;
    return numEntities();
}

void Registry::clearScene() {
    mBillBoards.clear();
    for (auto entity : mEntities) {
        if (entity.first != 0) {
            entityDestroyed(entity.first); // Pass the message on to the registry
            entity.second->destroy();
        }
    }
}
/**
 * @brief Make a generic game object with no components attached.
 * @param name Name of the gameobject. Leave blank if no name desired.
 * @return Returns the entity ID for use in adding components or other tasks.
 */
GLuint Registry::makeEntity(const QString &name, bool signal) {
    GLuint eID = nextAvailable();
    auto search = mEntities.find(eID);
    if (search != mEntities.end()) {
        search->second->newGeneration(eID, name);
    } else {
        if (name == "BillBoard")
            mEntities[eID] = std::make_shared<BillBoard>(eID, "BillBoard");
        else
            mEntities[eID] = std::make_shared<Entity>(eID, name);
    }
    if (signal)
        emit entityCreated(eID);
    return eID;
}

GLuint Registry::duplicateEntity(GLuint dupedEntity) {
    // Remember it also needs to be at the same parent level
    Ref<Entity> dupe = getEntity(dupedEntity);
    GLuint entityID = makeEntity(dupe->name());
    for (auto &pool : mPools) {
        if (pool.second->has(dupedEntity)) {
            pool.second->cloneComponent(dupedEntity, entityID);
        }
    }
    if (contains<Transform>(entityID)) {
        Transform &trans = getPool<Transform>()->get(entityID);
        setParent(entityID, trans.parentID);
        for (auto &child : trans.children) {
            GLuint newChild = duplicateEntity(child);
            child = newChild;
        }
    }
    return entityID;
}
/**
 * @brief Set the parent of a gameobject (or rather its transform component).
 * Note: Currently no support for setting an item to be a child of a previously created item, due to how we're currently inserting into items into the view.
 * For now, make sure you create items in the order you want them to be parented, i.e. Parent first, then Children.
 * @param eID
 * @param parentID
 */
void Registry::setParent(GLuint childID, int newParentID, bool fromEditor) {
    Transform &trans = getComponent<Transform>(childID);
    if (hasParent(childID)) // Make sure to remove the child from its old parent if it had one
        removeChild(trans.parentID, childID);
    trans.parentID = newParentID; // Set the new parent ID. Can be set to -1 if you want it to be independent again.
    if (newParentID != -1)
        addChild(newParentID, childID);
    if (!fromEditor)
        emit parentChanged(childID);
}

Transform &Registry::getParent(GLuint eID) {
    GLuint parentID = getComponent<Transform>(eID).parentID;
    return getComponent<Transform>(parentID);
}
bool Registry::hasParent(GLuint eID) {
    return getComponent<Transform>(eID).parentID != -1;
}
std::vector<GLuint> Registry::getChildren(GLuint eID) {
    return getComponent<Transform>(eID).children;
}

void Registry::addChild(const GLuint parentID, const GLuint childID) {
    auto &parent = getComponent<Transform>(parentID);
    parent.children.emplace_back(childID);
    getComponent<Transform>(childID).matrixOutdated = true;
}
void Registry::removeChild(const GLuint eID, const GLuint childID) {
    std::vector<GLuint> &children = getComponent<Transform>(eID).children;
    for (auto &child : children) {
        if (child == childID) {
            getComponent<Transform>(childID).matrixOutdated = true;
            std::swap(child, children.back());
            children.pop_back();
        }
    }
}
/**
 * @brief Updates the child parent hierarchy in case it's out of date
 */
void Registry::updateChildParent() {
    for (auto entity : getEntities()) // For every gameobject
    {
        if (contains<Transform>(entity.second->id())) {
            Transform &comp = getComponent<Transform>(entity.second->id());
            if (comp.parentID != -1) {                         // If this entity has a parent then,
                setParent(entity.second->id(), comp.parentID); // add this entity's ID to the parent's list of children.
            }
        }
    }
}

void Registry::makeSnapshot() {
    std::map<GLuint, Ref<Entity>> newEntityMap;
    for (auto entity : mEntities) {
        if (Ref<BillBoard> board = std::dynamic_pointer_cast<BillBoard>(entity.second)) {
            newEntityMap[entity.first] = board;
        } else {
            Ref<Entity> entt = entity.second;
            newEntityMap[entity.first] = entt;
        }
    }
    std::map<std::string, Ref<IPool>> snapPools;
    for (auto &pool : mPools) {
        snapPools[pool.first] = pool.second->clone();
    }

    mSnapshot = std::make_tuple(newEntityMap, mBillBoards, snapPools);
}

void Registry::loadSnapshot() {
    std::map<std::string, Ref<IPool>> tempPools;
    std::tie(mEntities, mBillBoards, tempPools) = mSnapshot;
    for (auto &pool : tempPools) {
        mPools[pool.first]->swap(pool.second);
    }
    for (auto &transform : getPool<Transform>()->data()) {
        transform.matrixOutdated = true;
    }
    // To-do: Make scene view load back to its pre-parented state if something is parented during play
}
