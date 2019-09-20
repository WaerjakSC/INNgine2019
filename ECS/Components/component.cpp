#include "component.h"

Component::~Component() {
}

CType Component::type() const {
    return mType;
}
// Compares the two component types for sorting purposes
//bool Component::operator<(const Component &comp) {
//    return (int)mType < (int)comp.mType;
//}

// Code initialization (Similar to Unity's void Start()), separating the constructor and code initialization
void Component::init() {
}