#ifndef BSPLINECURVE_H
#define BSPLINECURVE_H
#include "vector3d.h"
typedef gsl::Vector3D vec3;
class BSplineCurve: protected QOpenGLFunctions_4_1_Core  {
public:
    // default constructor
    BSplineCurve();
    BSplineCurve(std::vector<float> knots, std::vector<vec3> controlpoints, int degree = 2);
    std::vector<vec3> b;  // control points
    int n;                // n number of knots
    int d;                // d degrees
    std::vector<float> t; // knots

    // void setKnotsAndControlPoints(std::vector<float> knots, std::vector<vec3> points);

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

    void registerTrophies();

    void updatePath();

    //vec2 deBoor(float x);
};

#endif // BSPLINECURVE_H
