#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H
#include "component.h"
#include "matrix4x4.h"
#include "vertex.h"

class MaterialComponent;
class MeshComponent : public Component {
public:
    MeshComponent();

    virtual ~MeshComponent();

    virtual void update(float dt);
    virtual void init();
    virtual void draw(MaterialComponent *material);

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)

    GLuint mVerticeCount{0};
    GLuint mIndiceCount{0};
    GLenum mDrawType{0};

protected:
};

#endif // MESHCOMPONENT_H
