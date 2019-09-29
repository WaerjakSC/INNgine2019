#include "gameobject.h"

GameObject::GameObject(GLuint ID, std::string name) : mName(name), eID(ID) {
    types = CType::None;
}

GameObject::~GameObject() {
}

void GameObject::init() {
}

void GameObject::update() {
}
