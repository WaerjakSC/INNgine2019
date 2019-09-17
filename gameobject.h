#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Components/component.h"
#include "matrix4x4.h"

class GameObject {
public:
    GameObject(GLuint ID, std::string name = "");
    ~GameObject();

    std::string mName;

    GLuint eID;

    virtual void update();

    void init();
};

#endif // GAMEOBJECT_H
