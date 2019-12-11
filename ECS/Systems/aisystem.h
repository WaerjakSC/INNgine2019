#ifndef AISYSTEM_H
#define AISYSTEM_H

#include "bsplinecurve.h"
#include "components.h"
#include "isystem.h"
class Registry;

class AISystem : public QObject, public ISystem {
    Q_OBJECT
    using vec3 = gsl::Vector3D;

public:
    AISystem();

    virtual void update(DeltaTime dt = 0.016) override;
    void updatePlayOnly(DeltaTime dt = 0.016);
    void updateEditorOnly(DeltaTime dt = 0.016);

    void init();

    void draw();

    void resetTimers();
public slots:
    void setBSPlinePointX(double xIn);
    void setBSPlinePointY(double yIn);
    void setBSPlinePointZ(double zIn);
    void setHealth(int health);

private:
    // FSM npc
    std::optional<NPCevents> move(DeltaTime dt, AIComponent &ai, Transform &transform);
    void setControlPoints(std::vector<vec3> cps);
    void masterOfCurves();

    void death(const GLuint entityID);
    void goalReached(const GLuint entityID);
    //    void learn();
    void eventHandler(AIComponent &ai);

    BSplineCurve mCurve;
    Registry *registry;

    // FSM tower
    void detectEnemies(TowerComponent &ai, Sphere &sphere);
    void attack(TowerComponent &ai, Transform &trans);

    void notify(int notification);

    float elapsed_time;
    void spawnWave(DeltaTime dt);
    float waveCD{7.f}, curWaveCD{1.f}, spawnCD{0.5f}, curSpawnCD{0.f}, spawnDuration{5.f}, curSpawnDuration{0.f};
};

#endif // AISYSTEM_H
