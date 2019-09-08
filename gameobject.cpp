#include "gameobject.h"

GameObject::GameObject(GLuint ID, std::string name) : mName(name), eID(ID) {
}

GameObject::~GameObject() {
}

void GameObject::init() {
}

void GameObject::update() {
    //    for (auto *mesh : getMeshComponents()) {
    //        mesh->draw(mMatrix);
    //    }
}

//std::vector<MeshComponent *> GameObject::getMeshComponents() {
//    std::vector<MeshComponent *> temp;
//    // Temporary way to init meshcomponents, probably very inefficient
//    for (Component *comp : Components) {
//        auto *mesh = dynamic_cast<MeshComponent *>(comp);
//        if (mesh) {
//            temp.emplace_back(mesh);
//        }
//    }
//    return temp;
//}
/**
 * @brief GameObject::setShaders
 * Potentially temporary function (if we want to let a gameobject have more than one mesh component with different shaders, this won't be sufficient)
 */
//void GameObject::setShaders(Shader *shader) {
//    for (auto *mesh : getMeshComponents()) {
//        mesh->setShader(shader);
//    }
//}
