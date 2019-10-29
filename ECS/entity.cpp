#include "entity.h"

Entity::Entity(GLuint ID, const QString &text) : eID(ID), mName(text) {
}

Entity::~Entity() {
}

void Entity::setName(const QString &name) {
    mName = name;
    emit nameChanged(*this);
}

void Entity::setEntityData(const Entity &value) {
    eID = value.eID;
    mTypes = value.getTypes();
}
void Entity::newGeneration(GLuint id, const QString &text) {
    eID = id;
    mName = text;
    mIsDestroyed = false;
}

bool Entity::isDestroyed() const {
    return mIsDestroyed;
}

bool Entity::isEmpty() {
    return types() == CType::None;
}
void Entity::destroy() {
    mTypes = CType::None;
    mIsDestroyed = true;
    mName.clear();
    mGeneration++;
}
