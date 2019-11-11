#ifndef AISYSTEM_H
#define AISYSTEM_H

#include "bsplinecurve.h"
#include "components.h"
#include "isystem.h"
#include "registry.h"
#include <queue>

// For øyeblikket trenger vi FSM og bsplinecurve relevant stuff her
typedef gsl::Vector2D vec2;

class AIsystem : public ISystem {
public:
    AIsystem();
    Registry *reg;

    virtual void update(DeltaTime dt = 0.016) override;
    void setControlPoints(std::vector<vec3> cps);
    std::optional<NPCevents> move(float deltaTime);
    float t{0};
    int dir{1};
    void init(GLuint eID);
    void eventHandler();

    void draw();

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
