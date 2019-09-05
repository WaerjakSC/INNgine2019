#ifndef MATERIALCOMPONENT_H
#define MATERIALCOMPONENT_H

#include "component.h"
#include "vector3d.h"
#include <QOpenGLFunctions_4_1_Core>

struct MaterialData {
    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
    GLuint mTextureUnit{0}; //the actual texture to put into the uniform
};

/**
 * @brief The MaterialComponent class holds the shader, texture unit and objectcolor
 */
class MaterialComponent : public QOpenGLFunctions_4_1_Core, public Component {
public:
    MaterialComponent();

    void update(float dt);
    virtual void draw(gsl::Matrix4x4 &mMatrix);

    void setShader(class Shader *getShader);
    void setTextureUnit(const GLuint &textureUnit);
    virtual void setColor(const gsl::Vector3D &color);

    Shader *getShader() const;

protected:
    Shader *mShader{nullptr};
    MaterialData material;
};

#endif // MATERIALCOMPONENT_H
