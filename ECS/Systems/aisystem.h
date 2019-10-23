#ifndef AISYSTEM_H
#define AISYSTEM_H

#include "components.h"
#include "registry.h"
#include "isystem.h"

// For øyeblikket trenger vi FSM og bsplinecurve relevant stuff her

class AIsystem
{
public:
    AIsystem();
    Registry *reg = Registry::instance();

     // BSplineCurve
     void initVertexBufferObjects();
     void draw(GLint positionAttribute, GLint colorAttribute, GLint textureAttribute=-1);
     void setKnotsAndControlPoints(std::vector<float> knots, std::vector<vec3> points);
     vec3 evaluateBSpline(const BSplineCurve,int my, float x);
     int findKnotInterval(float x);
     vec3 evaluateBSpline(int degree, int startKnot, float x);

     // FSM
};

#endif // AISYSTEM_H
