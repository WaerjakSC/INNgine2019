#include "registry.h"
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

EInfo &Registry::getEntity(GLuint eID) {
    return get<EInfo>(eID);
}

void Registry::removeEntity(GLuint eID) {
    EInfo &info = get<EInfo>(eID);

    if (info.mIsDestroyed)
        return;
    if (isBillBoard(eID)) {
        removeBillBoardID(eID);
    }
    if (contains<Transform>(eID)) {
        setParent(eID, -1);
    }
    info.mIsDestroyed = true;
    info.mName.clear();
    info.mGeneration++;

    entityDestroyed(eID); // Pass the message on to each pool

    emit entityRemoved(eID);
}

GLuint Registry::nextAvailable() {
    std::vector<GLuint> entities = getPool<EInfo>()->entities();
    for (auto &entity : entities) {
        EInfo &info = get<EInfo>(entity);
        if (info.mIsDestroyed)
            return entity;
    }
    return numEntities();
}

void Registry::clearScene() {
    std::vector<GLuint> entities = getPool<EInfo>()->entities();
    for (auto &entity : entities) {
        if (entity != 0) {
            removeEntity(entity);
        }
    }
}

GLuint Registry::makeEntity(const QString &name, bool signal) {
    GLuint eID = nextAvailable();
    std::vector<GLuint> entities = getPool<EInfo>()->entities();

    //    if (name == "BillBoard")
    //        mEntities[eID] = std::make_unique<BillBoard>(eID, "BillBoard");
    if (contains<EInfo>(eID)) {
        newGeneration(eID, name);
    } else
        add<EInfo>(eID, name);
    if (signal)
        emit entityCreated(eID);
    return eID;
}

GLuint Registry::duplicateEntity(GLuint dupedEntity) {
    // Remember it also needs to be at the same parent level
    QString dupeName = get<EInfo>(dupedEntity).mName;
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

GLuint Registry::getSelectedEntity() const {
    return mSelectedEntity;
}

void Registry::setSelectedEntity(const GLuint selectedEntity) {
    mSelectedEntity = selectedEntity;
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
        if (contains<Transform>(entity)) {
            Transform &comp = get<Transform>(entity);
            if (comp.parentID != -1) {            // If this entity has a parent then,
                setParent(entity, comp.parentID); // add this entity's ID to the parent's list of children.
            }
        }
    }
}
void Registry::newGeneration(GLuint entityID, const QString &text) {
    EInfo &info = get<EInfo>(entityID);
    info.mName = text;
    info.mIsDestroyed = false;
}

bool Registry::isDestroyed(GLuint entityID) {
    EInfo &info = get<EInfo>(entityID);
    return info.mIsDestroyed;
}

void Registry::makeSnapshot() {
    std::map<std::string, IPool *> snapPools;
    for (auto &pool : mPools) {
        snapPools[pool.first] = pool.second->clone();
    }

    mSnapshot = std::make_tuple(mBillBoards, snapPools);
}

void Registry::loadSnapshot() {
    std::map<std::string, IPool *> tempPools;
    std::tie(mBillBoards, tempPools) = mSnapshot;
    mPools.clear();
    for (auto &pool : tempPools) {
        mPools[pool.first] = std::unique_ptr<IPool>(pool.second);
    }
    for (auto &transform : getPool<Transform>()->data()) {
        transform.matrixOutdated = true;
    }
}
