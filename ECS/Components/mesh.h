#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H
#include "component.h"
#include "matrix4x4.h"
#include "vertex.h"
#include <QOpenGLFunctions_4_1_Core>
class Material;
class Shader;
class Mesh : public Component {
public:
    Mesh();

    virtual ~Mesh();

    virtual void update(float dt = 0.0f);

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)

    GLuint mVerticeCount{0};
    GLuint mIndiceCount{0};
    GLenum mDrawType{0};

    void copyOpenGLData(const Mesh &comp);
};

#endif // MESHCOMPONENT_H
