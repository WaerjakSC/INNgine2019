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
     * @brief AIsystem update function.
     * @param dt
     */
    virtual void update(DeltaTime dt = 0.016) override;
    /**
     * @brief Play only update function.
     * @param dt
     */
    void updatePlayOnly(DeltaTime dt = 0.016);
    /**
     * @brief Editor only update function.
     * @param dt
     */
    void updateEditorOnly(DeltaTime dt = 0.016);

    void init();

    void draw();

    /** A timer helper function
     *
     * @param ai
     * @param dt
     */
    void deathTimer(AIComponent &ai, DeltaTime dt = 0.016);

    /// A vector containing the entity id's of the dead npcs
    std::vector<GLuint> deadAI;

    void resetTimers();
public slots:
    void setBSPlinePointX(double xIn);
    void setBSPlinePointY(double yIn);
    void setBSPlinePointZ(double zIn);
    void setHealth(int health);

private:
    /**
     * @brief Moves the NPCs along the bspline from spawn to endpoint.
     * @param dt
     * @param ai
     * @param transform
     * @return
     */
    std::optional<NPCevents> move(DeltaTime dt, AIComponent &ai, Transform &transform);
    /**
     * @brief Sets the control points for a bspline.
     * @param cps
     */
    void setControlPoints(std::vector<vec3> cps);
    /**
     * @brief Updates trophies and path.
     */
    void masterOfCurves();
    /**
     * @brief Handles NPC death.
     *        Adds the npc to a deadAI vector, updates player gold and emits particles.
     * @param entityID
     */
    void death(const GLuint entityID);
    /**
     * @brief Handles when the NPCs reaches end of path.
     * @param entityID
     */
    void goalReached(const GLuint entityID);
    /**
     * @brief Handles the events used for NPC FSM.
     * @param ai
     */
    void eventHandler(AIComponent &ai);

    BSplineCurve mCurve;
    Registry *registry;

    /**
     * @brief NPC detection for the towers.
     * @param ai
     * @param sphere
     */
    void detectEnemies(TowerComponent &ai, Sphere &sphere);
    /**
     * @brief Tower attack functionality, launches a projectile towards NPC position.
     * @param ai
     * @param trans
     */
    void attack(TowerComponent &ai, Transform &trans);

    void notify(int notification);
    /**
     * @brief Spawns NPCs.
     * @param dt
     */
    void spawnWave(DeltaTime dt);
    float elapsed_time;
    float waveCD{7.f}, curWaveCD{1.f}, spawnCD{0.5f}, curSpawnCD{0.f}, spawnDuration{5.f}, curSpawnDuration{0.f}, curTimerCD{2.f}, curTimer{0.f};
    /**
     * Remove bullets after a certain amount of time, to avoid stacking up a ton of missed bullets.
     * @param dt
     */
    void bulletLifeTime(DeltaTime dt);
};

#endif // AISYSTEM_H
