#include "aisystem.h"
#include "components.h"
#include "gsl_math.h"
#include "movementsystem.h"
#include "registry.h"
namespace cjk {
AISystem::AISystem()
{
}

/**
 * @brief AIsystem::update, runs the highly sophisticated AI cortex
 * @param dt
 */
void AISystem::update(DeltaTime)
{
}

void AISystem::updatePlayOnly(DeltaTime dt)
{
    // Run the eventHandler incase of events
    if (curWaveCD >= 0.f)
        curWaveCD -= dt;
    if (curWaveCD <= 0.f) {
        spawnWave(dt);
        if (curSpawnDuration >= spawnDuration) {
            curWaveCD = waveCD;
            curSpawnDuration = 0.f;
        }
    }
    // Currently only set up for one entity
    auto view{registry->view<Transform, AIComponent>()};
    for (auto entity : view) {
        auto [transform, ai] = view.get<Transform, AIComponent>(entity);
        eventHandler(ai);
        std::optional<NPCevents> event;
        switch (ai.state) {
        case MOVE:
            event = move(dt, ai, transform);
            if (event) {
                ai.notification_queue.push(event.value());
            }
            break;
        case DEATH:
            // Whatever happens when gnomes die
            death(entity);
            break;
        case GOAL_REACHED:
            // implement this for folder
            goalReached(entity);
            break;
        }
    }

    auto twrview{registry->view<TowerComponent>()};
    for (auto entity : twrview) {
        auto &aicomponent{twrview.get(entity)};

        switch (twrstate) {
        case IDLE:
            // scanning for monsters
            detectEnemies(aicomponent);
            break;
        case ATTACK:
            // kill goblin
            attack(aicomponent);
            break;
        case COOLDOWN:
            // cooldown
            break;
        }
    }
}

void AISystem::updateEditorOnly(DeltaTime)
{
    draw();
}
void AISystem::resetTimers()
{
    curWaveCD = 1.f;
    curSpawnCD = 0.f;
    curSpawnDuration = 0.f;
}
/**
 * @brief AIsystem::learn, flips the direction and updates path
 */
//void AISystem::learn() {
//    dir = -dir;
//    if (updatePath) {
//        mCurve.updatePath();
//        //            mCurve.updateTrophies();
//        updatePath = false;
//    }
//    state = MOVE;
//}
void AISystem::spawnWave(DeltaTime dt)
{
    curSpawnDuration += dt;
    if (curSpawnCD >= 0.f)
        curSpawnCD -= dt;
    if (curSpawnCD <= 0.f) {
        auto factory{ResourceManager::instance()};
        factory->makeEnemy();
        curSpawnCD = spawnCD;
    }
}
void AISystem::detectEnemies(TowerComponent &ai)
{
    // if(!(list.empty())
    // pick random from list
    // save it in targetID
    // twrstate = ATTACK;
}

void AISystem::attack(TowerComponent &ai)
{
    // We already have the targetID here
}

/**
 * @brief AIsystem::eventHandler, event handling
 */
void AISystem::eventHandler(AIComponent &ai)
{
    while (!ai.notification_queue.empty()) {
        auto event = ai.notification_queue.front();
        switch (event) {
        case ENDPOINT_ARRIVED:
            ai.state = GOAL_REACHED;
            break;
        case DAMAGE_TAKEN:
            if (ai.health <= 0) {
                ai.state = DEATH;
            }
            break;
        default:
            break;
        }
        ai.notification_queue.pop();
    }
}

/**
 * @brief AIsystem::draw
 */
void AISystem::draw()
{
    mCurve.draw();
}

// follow b-spline from start of path to end of path
// if end of path is reached,
// remove 1 LP (lifepoint) from player's base

/* position = deBoor(t)
     * draw NPC
     * if(endpoint arrived)
     *  notify(endpoint_arrived)
     *  state(GOAL_REACHED);
     * if(AIcomponent.hp >= 0)
     *  state(DEATH)
     *
     *
     */

/**
 * @brief AIsystem::move, moves the NPC along the path
 * @param deltaT
 * @return
 */
std::optional<NPCevents> AISystem::move(DeltaTime dt, AIComponent &ai, Transform &transform)
{
    float &t = ai.pathT; // shortcut
    t += dt * ai.moveSpeed;
    bool endPoint{0.98f <= t || t < 0.f};

    if (endPoint)
        t = gsl::clamp(t, 0.f, 1.f);

    auto p{mCurve.eval(t)};
    transform.localPosition = p;
    transform.matrixOutdated = true;

    if (endPoint) {
        // remove 1 hp from player
        return ENDPOINT_ARRIVED;
    }

    return {};
}

/**
 * @brief AIsystem::init, initializes NPC and curve
 * @param eID
 */
void AISystem::init()
{
    mCurve.init();
    masterOfCurves();
}

/**
 * @brief AIsystem::death, todo
 */
void AISystem::death(const GLuint entityID)
{
    // hp <= 0
    // particles
    // gold++
    // delete entity
    registry->removeEntity(entityID);
}

/**
 * @brief AIsystem::goalReached, todo
 */
void AISystem::goalReached(const GLuint entityID)
{
    // endpoint reached
    // remove 1LP from player
    // delete entity
    registry->removeEntity(entityID);
}
/**
 * @brief AIsystem::masterOfCurves, helper function that updates trophies and path
 */
void AISystem::masterOfCurves()
{
    mCurve.updateTrophies();
    mCurve.updatePath();
}
void AISystem::setHealth(int health)
{
    auto ai{registry->get<AIComponent>(registry->getSelectedEntity())};
    ai.health = health;
}
void AISystem::setBSPlinePointX(double xIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &bspline{registry->get<BSplinePoint>(entityID)};
    bspline.location.x = xIn;
    masterOfCurves();
}

void AISystem::setBSPlinePointY(double yIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &bspline{registry->get<BSplinePoint>(entityID)};
    bspline.location.y = yIn;
    masterOfCurves();
}

void AISystem::setBSPlinePointZ(double zIn)
{
    GLuint entityID{registry->getSelectedEntity()};
    auto &bspline{registry->get<BSplinePoint>(entityID)};
    bspline.location.z = zIn;
    masterOfCurves();
}
/**
 * @brief AIsystem::setControlPoints
 * @param cps
 */
void AISystem::setControlPoints(std::vector<vec3> cps)
{
    mCurve.setControlPoints(cps);
}

} // namespace cjk
