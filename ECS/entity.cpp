#include "entity.h"

Entity::Entity(GLuint ID, const QString &text) : QStandardItem() {
    setText(text);
    eID = ID;
}

Entity::Entity() {
}

Entity::~Entity() {
}

void Entity::setEntityData(const Entity &value, int role) {
    QStandardItem::setData(value.data(), role);
    eID = value.eID;
    mTypes = value.getTypes();
}

Entity *Entity::cloneEntity() const {
    Entity *item = new Entity(eID, text());
    item->types() = mTypes;
    return item;
}
