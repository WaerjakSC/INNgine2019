#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H
#include "component.h"
#include "material.h"
#include "matrix4x4.h"
#include "vertex.h"
#include <QOpenGLFunctions_4_1_Core>

struct meshData {
    meshData() {}
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
};

class MeshComponent : public QOpenGLFunctions_4_1_Core {
public:
    MeshComponent();
    MeshComponent(std::string filename);
    virtual ~MeshComponent();

    virtual void init();
    virtual void draw(gsl::Matrix4x4 &mMatrix);

    Material mMaterial;

    void readFile(std::string filename);

private:
    meshData *mesh{nullptr};

protected:
    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)
};

#endif // MESHCOMPONENT_H
