#include "gameobject.h"

GameObject::GameObject(std::string name) : mName(name) {
    mMatrix.setToIdentity();
}
void GameObject::init() {
    for (auto *mesh : getMeshComponents()) {
        mesh->init();
    }
}
void GameObject::update() {
    for (auto *mesh : getMeshComponents()) {
        mesh->draw(mMatrix);
    }
}
/**
 * @brief GameObject::addComponent
 * @param comp
 * Not sure about checking for uniqueness and stuff for components that you only ever want one of...
 */
//template <typename compType> // In theory this template should allow the adding of any class that inherits from the Component class
void GameObject::addComponent(Component *comp) {
    Components.push_back(comp);
}
std::vector<MeshComponent *> GameObject::getMeshComponents() {
    std::vector<MeshComponent *> temp;
    // Temporary way to init meshcomponents, probably very inefficient
    for (Component *comp : Components) {
        auto *mesh = dynamic_cast<MeshComponent *>(comp);
        if (mesh) {
            temp.emplace_back(mesh);
        }
    }
    return temp;
}
/**
 * @brief GameObject::setShaders
 * Potentially temporary function (if we want to let a gameobject have more than one mesh component with different shaders, this won't be sufficient)
 */
void GameObject::setShaders(Shader *shader) {
    for (auto *mesh : getMeshComponents()) {
        mesh->setShader(shader);
    }
}
