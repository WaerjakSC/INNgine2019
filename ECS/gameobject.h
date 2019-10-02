#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "components.h"
#include "matrix4x4.h"

class GameObject {
public:
    GameObject(GLuint ID, std::string name = "");
    virtual ~GameObject();

    std::string mName;

    GLuint eID;

    CType types{CType::None};

    virtual void update();

    void init();
};

#endif // GAMEOBJECT_H
