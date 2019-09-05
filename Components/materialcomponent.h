#ifndef MATERIALCOMPONENT_H
#define MATERIALCOMPONENT_H

#include "component.h"
#include "vector3d.h"
#include <QOpenGLFunctions_4_1_Core>

struct MaterialData {
    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
    GLuint mTextureUnit{0}; //the actual texture to put into the uniform
};
class Shader;
/**
 * @brief The MaterialComponent class holds the shader, texture unit and objectcolor
 */
class MaterialComponent : public Component {
public:
    MaterialComponent();

    void update(float dt);
    virtual void draw(gsl::Matrix4x4 &mMatrix);

    void setTextureUnit(const GLuint &textureUnit);
    virtual void setColor(const gsl::Vector3D &color);

    void setShader(Shader *getShader);
    Shader *getShader() const;

protected:
    Shader *mShader{nullptr};
    MaterialData material;
};

#endif // MATERIALCOMPONENT_H
