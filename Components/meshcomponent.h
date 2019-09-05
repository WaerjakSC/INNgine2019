#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H
#include "component.h"
#include "material.h"
#include "matrix4x4.h"
#include "vertex.h"
#include <QOpenGLFunctions_4_1_Core>

struct meshData {
    meshData() = default;
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
};
class MaterialComponent;
class MeshComponent : public QOpenGLFunctions_4_1_Core, public Component {
public:
    MeshComponent();
    MeshComponent(meshData *mMesh);

    virtual ~MeshComponent();
    virtual void update(float dt);
    virtual void init();
    virtual void draw(MaterialComponent *material);

    void setShader(Shader *shader);

    meshData *getMesh() const;

    void setMesh(meshData *mesh);

protected:
    meshData *mMesh{nullptr}; // could be a shared_ptr? not sure about performance though

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)
};

#endif // MESHCOMPONENT_H
