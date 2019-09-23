#ifndef MATERIALCOMPONENT_H
#define MATERIALCOMPONENT_H

#include "component.h"
#include "shader.h"
#include "vector3d.h"
#include <QOpenGLFunctions_4_1_Core>

struct MaterialData {
    MaterialData() {} // Default constructor
    MaterialData(gsl::Vector3D color, GLuint texture) {
        mObjectColor = color;
        mTextureUnit = texture;
    }
    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
    GLuint mTextureUnit{0}; //the actual texture to put into the uniform
};

class Shader;
/**
 * @brief The MaterialComponent class holds the shader, texture unit and objectcolor
 */
class Material : public Component {
public:
    friend class RenderSystem;
    Material();
    Material(ShaderType type, GLuint texture = 0, gsl::Vector3D color = gsl::Vector3D(1));
    Material(ShaderType type, MaterialData data);

    void update(float dt = 0.0f);

    void setShader(ShaderType getShader);
    void setTextureUnit(const GLuint &textureUnit);
    virtual void setColor(const gsl::Vector3D &color);

    ShaderType getShader() const;
    //public slots:
    //    void updatedMatrix(gsl::Matrix4x4 *matrix) { mMatrix = matrix; }

protected:
    MaterialData material;
    ShaderType mShader;
};

#endif // MATERIALCOMPONENT_H