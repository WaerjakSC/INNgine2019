#ifndef AISYSTEM_H
#define AISYSTEM_H

#include "bsplinecurve.h"
#include "components.h"
#include "isystem.h"
#include "registry.h"
#include <queue>

// For øyeblikket trenger vi FSM og bsplinecurve relevant stuff her
typedef gsl::Vector2D vec2;

class AISystem : public QObject, public ISystem {
    Q_OBJECT
public:
    AISystem();
    Registry *reg;

    virtual void update(DeltaTime dt = 0.016) override;
    void setControlPoints(std::vector<vec3> cps);
    std::optional<NPCevents> move(float deltaTime);
    float t{0};
    int dir{1};
    void init(GLuint eID);
    void eventHandler();

    void draw();
    void masterOfCurves();

public slots:
    void setBSPlinePointX(double xIn);
    void setBSPlinePointY(double yIn);
    void setBSPlinePointZ(double zIn);
    void setHealth(int health);

private:
    // FSM
    void move();
    void death();
    void goalReached();
    void learn();

    bool updatePath = false;
    BSplineCurve mCurve;
    Registry *registry = Registry::instance();



    GLuint NPC;

    TWRstates twrstate{IDLE};
    NPCstates state{MOVE};
    void notify(int notification);
    std::queue<int> notification_queue;

    float elapsed_time;
};

#endif // AISYSTEM_H
