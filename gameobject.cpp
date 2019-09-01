#include "gameobject.h"

GameObject::GameObject(std::string name) : objectName(name) {
    mMatrix.setToIdentity();
}

void GameObject::update() {
}
/**
 * @brief GameObject::addComponent
 * @param comp
 * Not sure about checking for uniqueness and stuff for components that you only ever want one of...
 */
template <typename compType> // In theory this template should allow the adding of any class that inherits from the Component class
void GameObject::addComponent(compType comp) {
    Components.push_back(std::make_unique<compType>(comp));
}
