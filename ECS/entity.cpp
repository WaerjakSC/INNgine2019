#include "entity.h"

Entity::Entity(GLuint ID, const QString &text) : QObject(), eID(ID), mName(text) {
}

Entity::Entity(const Entity &other) : QObject() {
    mName = other.name();
    eID = other.id();
    mGeneration = other.mGeneration;
    mIsDestroyed = other.mIsDestroyed;
}

Entity::~Entity() {
}

void Entity::setName(const QString &name) {
    mName = name;
    emit nameChanged(*this);
}

void Entity::setEntityData(const Entity &value) {
    eID = value.eID;
}

Scope<Entity> Entity::clone() {
    return std::make_unique<Entity>(*this);
}
void Entity::newGeneration(GLuint id, const QString &text) {
    eID = id;
    mName = text;
    mIsDestroyed = false;
}

bool Entity::isDestroyed() const {
    return mIsDestroyed;
}

void Entity::destroy() {
    mIsDestroyed = true;
    mName.clear();
    mGeneration++;
}
