#ifndef XYZ_H
#define XYZ_H

#include "visualobject.h"
#include <QOpenGLFunctions_4_1_Core>

class XYZ : public VisualObject {
public:
    XYZ();
    ~XYZ() override;
    void init() override;
    void draw() override;

private:
};

#endif // XYZ_H
