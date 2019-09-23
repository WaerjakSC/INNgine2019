#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H
#include "component.h"
#include "matrix4x4.h"
#include "vertex.h"
#include <QOpenGLFunctions_4_1_Core>
class Material;
class Shader;
struct meshData {
    meshData() = default;
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
    void Clear() {
        mVertices.clear();
        mIndices.clear();
    }
};
class Mesh : public Component {
public:
    Mesh();
    Mesh(GLenum drawType, GLuint vertices = 0, GLuint indices = 0);
    Mesh(GLenum drawType, meshData data);

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
