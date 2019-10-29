#ifndef AISYSTEM_H
#define AISYSTEM_H

#include "components.h"
#include "isystem.h"
#include "registry.h"
#include <queue>

// For øyeblikket trenger vi FSM og bsplinecurve relevant stuff her

class AIsystem : public ISystem {
public:
    AIsystem();
    Registry *reg;

    virtual void update(float deltaTime = 0.016);

    // BSplineCurve
    void initVertexBufferObjects();
    void draw(GLint positionAttribute, GLint colorAttribute, GLint textureAttribute = -1);
    void setKnotsAndControlPoints(std::vector<float> knots, std::vector<vec3> points);
    vec3 evaluateBSpline(const BSplineCurve &bspline, int my, float x);
    int findKnotInterval(float x);
    vec3 evaluateBSpline(int degree, int startKnot, float x);

    // FSM
    void move();
    NPCstates state;
    void notify(int notification);
    std::queue<int> notification_queue;
};

#endif // AISYSTEM_H
