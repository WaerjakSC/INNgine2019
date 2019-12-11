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

    /**
     * @brief update
     * @param dt
     */
    virtual void update(DeltaTime dt = 0.016) override;
    /**
     * @brief updatePlayOnly
     * @param dt
     */
    void updatePlayOnly(DeltaTime dt = 0.016);
    /**
     * @brief updateEditorOnly
     * @param dt
     */
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
    /**
     * @brief move, moves the NPCs along the bspline from spawn to endpoint
     * @param dt
     * @param ai
     * @param transform
     * @return
     */
    std::optional<NPCevents> move(DeltaTime dt, AIComponent &ai, Transform &transform);
    /**
     * @brief setControlPoints
     * @param cps
     */
    void setControlPoints(std::vector<vec3> cps);
    /**
     * @brief masterOfCurves
     */
    void masterOfCurves();
    /**
     * @brief death, NPC death
     * @param entityID
     */
    void death(const GLuint entityID);
    /**
     * @brief goalReached, handles when the NPCs reaches end of path
     * @param entityID
     */
    void goalReached(const GLuint entityID);
    /**
     * @brief eventHandler
     * @param ai
     */
    void eventHandler(AIComponent &ai);

    BSplineCurve mCurve;
    Registry *registry;

    /**
     * @brief detectEnemies, NPC detection for the towers
     * @param ai
     * @param sphere
     */
    void detectEnemies(TowerComponent &ai, Sphere &sphere);
    /**
     * @brief attack, tower attack functionality, launches a projectile towards NPC position
     * @param ai
     * @param trans
     */
    void attack(TowerComponent &ai, Transform &trans);

    void notify(int notification);
    /**
     * @brief spawnWave, spawns NPCs
     * @param dt
     */
    void spawnWave(DeltaTime dt);
    float elapsed_time;
    float waveCD{7.f}, curWaveCD{1.f}, spawnCD{0.5f}, curSpawnCD{0.f}, spawnDuration{5.f}, curSpawnDuration{0.f};
};

#endif // AISYSTEM_H
