#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Components/component.h"
#include "matrix4x4.h"

class GameObject {
public:
    GameObject(std::string name);

    gsl::Matrix4x4 mMatrix;

    std::string objectName;

    virtual void update();

    template <typename compType>
    void addComponent(compType comp);

private:
    std::vector<std::unique_ptr<Component>> Components; // Needs testing, not sure if it needs to be a different type
};

#endif // GAMEOBJECT_H
