#ifndef BSPLINECURVE_H
#define BSPLINECURVE_H
#include "core.h"
#include "vector3d.h"
#include <QOpenGLFunctions_4_1_Core>
namespace cjk {
class ColorShader;
class BSplineCurve : protected QOpenGLFunctions_4_1_Core {
    using vec3 = gsl::Vector3D;

public:
    // default constructor
    BSplineCurve(int degree = 2);

    std::vector<vec3> b;  // control points
    int n;                // n number of knots
    int d;                // d degrees
    std::vector<float> t; // knots
    vec3 evaluateBSpline(int my, float x) const;

    int splineResolution = 50.f;
    bool debugLine = true;
    void draw();
    void init();

    GLuint mVAO;
    GLuint mVBO;

    int findKnotInterval(float x);
    std::vector<float> findKnots() const;
    void setControlPoints(const std::vector<vec3> &cp);

    int getMy(float x) const;
    vec3 eval(float x) const;

    void updateTrophies();
    void updatePath(bool init = false);

private:
    cjk::Ref<ColorShader> debugShader;
};
} // namespace cjk

#endif // BSPLINECURVE_H
