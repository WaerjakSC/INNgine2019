#ifndef AISYSTEM_H
#define AISYSTEM_H

#include "components.h"
#include "isystem.h"
#include "registry.h"
#include "bsplinecurve.h"
#include <queue>

// For øyeblikket trenger vi FSM og bsplinecurve relevant stuff her
typedef gsl::Vector2D vec2;

class AIsystem : public ISystem {
public:
    AIsystem();
    Registry *reg;

    virtual void update(DeltaTime dt = 0.016) override;

    // BSplineCurve
    void initVertexBufferObjects();
    void draw(GLint positionAttribute, GLint colorAttribute, GLint textureAttribute = -1);
    void setKnotsAndControlPoints(std::vector<float> knots, std::vector<vec3> points);
    vec3 evaluateBSpline(const BSplinePoint &bspline, int my, float x);
    int findKnotInterval(float x);
    vec3 evaluateBSpline(int degree, int startKnot, float x);

    vec2 deBoor(float x);

     std::optional<NPCevents> move(float deltaTime);
     float t{0};
     int dir{1};
     void init(GLuint eID);
     void eventHandler();
private:
    // FSM
    void move();
    void death();
    void goalReached();

    bool updatePath = false;
    BSplineCurve mCurve;

    GLuint NPC;

    NPCstates state;
    void notify(int notification);
    std::queue<int> notification_queue;

    float elapsed_time;
};

#endif // AISYSTEM_H
