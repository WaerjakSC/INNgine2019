#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include "component.h"

class TransformComponent : public Component {
public:
    TransformComponent();
    virtual void update(float dt = 0.0f);

    //signals:
    //    void matrixChanged(const gsl::Matrix4x4 matrix);

    gsl::Matrix4x4 *matrix();

private:
    gsl::Matrix4x4 mMatrix; // not sure how to handle the model matrix yet
    // Probably want to only update it if you change a value like "ActorLocation" or something.
    // At some point we need to connect the mMatrix between at least the material (shader) component and the transform component somehow.
    // For now the transformcomponent mMatrix serves no purpose.
};

#endif // TRANSFORMCOMPONENT_H
