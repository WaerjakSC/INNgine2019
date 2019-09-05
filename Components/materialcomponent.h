#ifndef MATERIALCOMPONENT_H
#define MATERIALCOMPONENT_H

#include "component.h"
#include "vector3d.h"
/**
 * @brief The MaterialComponent class holds the shader, texture unit and objectcolor
 */
class MaterialComponent : public Component {
public:
    MaterialComponent();

    void update(float dt);

    void setShader(class Shader *shader);
    void setTextureUnit(const GLuint &textureUnit);
    void setColor(const gsl::Vector3D &color);

private:
    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
    GLuint mTextureUnit{0}; //the actual texture to put into the uniform
    Shader *mShader{nullptr};
};

#endif // MATERIALCOMPONENT_H
