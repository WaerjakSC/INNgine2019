#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H
#include "component.h"
#include "matrix4x4.h"
#include "vertex.h"
#include <QOpenGLFunctions_4_1_Core>
class MaterialComponent;
class Shader;
class MeshComponent : public Component {
public:
    MeshComponent();

    virtual ~MeshComponent();

    virtual void update(float dt = 0.0f);

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)

    GLuint mVerticeCount{0};
    GLuint mIndiceCount{0};
    GLenum mDrawType{0};

    void setShader(Shader *shader);

    void copyOpenGLData(const MeshComponent &comp);

private:
    Shader *mShader{nullptr};
};

#endif // MESHCOMPONENT_H
