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
