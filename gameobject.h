#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Components/meshcomponent.h"
#include "matrix4x4.h"

class GameObject {
public:
    GameObject(std::string name);

    gsl::Matrix4x4 mMatrix;

    std::string mName;

    GLuint eID;
    virtual void update();

    void addComponent(Component *comp);

    void init();

    std::vector<MeshComponent *> getMeshComponents();
    void setShaders(Shader *shader);

private:
    GLuint mComponents[8];
    std::vector<Component *> Components; // Needs testing, not sure if it needs to be a different type
};

#endif // GAMEOBJECT_H
