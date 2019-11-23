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

Entity *Registry::getEntity(GLuint eID) {
    auto search = mEntities.find(eID);
    if (search != mEntities.end())
        return search->second.get();
    return nullptr;
}

void Registry::removeEntity(GLuint eID) {
    Entity *entt = mEntities.find(eID)->second.get();
    if (entt->isDestroyed())
        return;
    if (isBillBoard(eID)) {
        removeBillBoardID(eID);
    }
    if (contains<Transform>(eID)) {
        setParent(eID, -1);
    }
    entityDestroyed(eID); // Pass the message on to the registry
    entt->destroy();      // doesn't actually destroy the entity object, simply clears the data and increments the generation variable

    emit entityRemoved(eID);
}

GLuint Registry::nextAvailable() {
    for (auto &entity : mEntities)
        if (entity.second->isDestroyed())
            return entity.first;
    return numEntities();
}

void Registry::clearScene() {
    for (auto &entity : mEntities) {
        if (entity.first != 0) {
            removeEntity(entity.first);
        }
    }
}

GLuint Registry::makeEntity(const QString &name, bool signal) {
    GLuint eID = nextAvailable();
    auto search = mEntities.find(eID);
    if (search != mEntities.end()) {
        search->second->newGeneration(eID, name);
    } else {
        if (name == "BillBoard")
            mEntities[eID] = std::make_unique<BillBoard>(eID, "BillBoard");
        else
            mEntities[eID] = std::make_unique<Entity>(eID, name);
    }
    if (signal)
        emit entityCreated(eID);
    return eID;
}

GLuint Registry::duplicateEntity(GLuint dupedEntity) {
    // Remember it also needs to be at the same parent level
    QString dupeName = getEntity(dupedEntity)->name();
    GLuint entityID = makeEntity(dupeName);
    for (auto &pool : mPools) {
        if (pool.second->has(dupedEntity)) {
            pool.second->cloneComponent(dupedEntity, entityID);
        }
    }
    if (contains<Transform>(dupedEntity) && hasParent(dupedEntity)) {
        Transform &trans = getPool<Transform>()->get(entityID);
        setParent(entityID, trans.parentID);
        for (auto &child : trans.children) {
            GLuint newChild = duplicateEntity(child);
            child = newChild;
        }
    }
    return entityID;
}

void Registry::setParent(GLuint childID, int newParentID, bool fromEditor) {
    Transform &trans = get<Transform>(childID);
    if (hasParent(childID)) // Make sure to remove the child from its old parent if it had one
        removeChild(trans.parentID, childID);
    trans.parentID = newParentID; // Set the new parent ID. Can be set to -1 if you want it to be independent again.
    if (newParentID != -1)
        addChild(newParentID, childID);
    if (!fromEditor)
        emit parentChanged(childID);
}

Transform &Registry::getParent(GLuint eID) {
    GLuint parentID = get<Transform>(eID).parentID;
    return get<Transform>(parentID);
}
bool Registry::hasParent(GLuint eID) {
    return get<Transform>(eID).parentID != -1;
}
std::vector<GLuint> Registry::getChildren(GLuint eID) {
    return get<Transform>(eID).children;
}

Entity *Registry::getSelectedEntity() const {
    return mSelectedEntity;
}

void Registry::setSelectedEntity(const GLuint selectedEntity) {
    mSelectedEntity = getEntity(selectedEntity);
}

void Registry::addChild(const GLuint parentID, const GLuint childID) {
    auto &parent = get<Transform>(parentID);
    parent.children.emplace_back(childID);
    get<Transform>(childID).matrixOutdated = true;
}
void Registry::removeChild(const GLuint eID, const GLuint childID) {
    std::vector<GLuint> &children = get<Transform>(eID).children;
    for (auto &child : children) {
        if (child == childID) {
            get<Transform>(childID).matrixOutdated = true;
            std::swap(child, children.back());
            children.pop_back();
        }
    }
}

void Registry::updateChildParent() {
    for (auto &entity : getEntities()) // For every entity
    {
        if (contains<Transform>(entity.second->id())) {
            Transform &comp = get<Transform>(entity.second->id());
            if (comp.parentID != -1) {                         // If this entity has a parent then,
                setParent(entity.second->id(), comp.parentID); // add this entity's ID to the parent's list of children.
            }
        }
    }
}

void Registry::makeSnapshot() {
    std::map<GLuint, Scope<Entity>> newEntityMap;
    for (auto &entity : mEntities) {
        if (isBillBoard(entity.second->id())) {
            newEntityMap[entity.first] = entity.second->clone();
        } else {
            newEntityMap[entity.first] = entity.second->clone();
        }
    }
    std::map<std::string, Scope<IPool>> snapPools;
    for (auto &pool : mPools) {
        snapPools[pool.first] = pool.second->clone();
    }

    mSnapshot = std::make_tuple(newEntityMap, mBillBoards, snapPools);
}

void Registry::loadSnapshot() {
    std::map<std::string, Scope<IPool>> tempPools;
    std::tie(mEntities, mBillBoards, tempPools) = mSnapshot;
    mPools.clear();
    for (auto &pool : tempPools) {
        mPools[pool.first] = std::move(pool.second);
    }
    for (auto &transform : getPool<Transform>()->data()) {
        transform.matrixOutdated = true;
    }
    // To-do: Make scene view load back to its pre-parented state if something is parented during play
}
