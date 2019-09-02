#ifndef XYZ_H
#define XYZ_H

#include "Components/meshcomponent.h"
#include <QOpenGLFunctions_4_1_Core>

class XYZ : public MeshComponent {
public:
    XYZ();
    ~XYZ() override;
    void init() override;
    void draw(gsl::Matrix4x4 &mMatrix) override;

private:
};

#endif // XYZ_H
