#include "gameobject.h"

GameObject::GameObject(GLuint ID, std::string name) : mName(name), eID(ID) {
    // Reserve 7 slots and set all to -1 to indicate that the entity has no components yet.
    for (std::size_t i = 0; i < 7; i++) {
        mComponentsID.push_back(-1);
    }
    mMatrix.setToIdentity();
}

GameObject::~GameObject() {
}

bool GameObject::hasComponent(const CType type) {
    return mComponentsID.at((int)type) > -1;
}
void GameObject::init() {
}

int GameObject::getComponentIndex(const CType type) {
    return mComponentsID.at(type);
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
