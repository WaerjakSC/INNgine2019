#include "aisystem.h"
#include "gsl_math.h"
#include "registry.h"
#include "resourcemanager.h"

AISystem::AISystem() : registry(Registry::instance())
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
        case NPCstates::MOVE:
            event = move(dt, ai, transform);
            if (event) {
                ai.notification_queue.push(event.value());
            }
            break;
        case NPCstates::DEATH:
            // Whatever happens when gnomes die
            death(entity);
            break;
        case NPCstates::GOAL_REACHED:
            // implement this for folder
            goalReached(entity);
            break;
        }
    }

    auto towerView{registry->view<TowerComponent, Sphere, Transform>()};
    for (auto entity : towerView) {
        auto [ai, sphere, transform]{towerView.get<TowerComponent, Sphere, Transform>(entity)};
        switch (ai.state) {
        case TowerStates::IDLE:
            // scanning for monsters
            detectEnemies(ai, sphere);
            break;
        case TowerStates::ATTACK:
            if (ai.curCooldown >= 0.f)
                ai.curCooldown -= dt;
            if (ai.curCooldown <= 0.f) {
                attack(ai, transform);
                ai.curCooldown = ai.cooldown;
            }
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
void AISystem::detectEnemies(TowerComponent &ai, Sphere &sphere)
{
    if (!sphere.overlappedEntities.empty()) {
        ai.targetID = sphere.overlappedEntities.getList()[0];
        ai.state = TowerStates::ATTACK;
    }
    else {
        ai.state = TowerStates::IDLE;
    }
}

void AISystem::attack(TowerComponent &ai, Transform &t)
{
    // Splash/Rocket type
    // BSpline from Tower to Enemy entity,
    // 2 Controlpoints -> First control point is offset by some value on y axis, 2nd is set at target location

    // Standard/Projectile type
    if (registry->contains<Transform>(ai.targetID)) {
        auto &trans{registry->get<Transform>(ai.targetID)};
        GLuint bulletID = ResourceManager::instance()->makeOctBall("projectile", 1);
        vec3 velocity{(trans.localPosition - t.localPosition).normalized()}; // get the vector (line) from tower to enemy, normalize to get the general direction.
        registry->add<Bullet>(bulletID, velocity, ai.damage, ai.projectileSpeed);
        registry->add<Sphere>(bulletID, vec3{0}, .25f, false);
        registry->get<Transform>(bulletID).localPosition = t.position;
        registry->get<Transform>(bulletID).localScale = vec3{0.25, 0.25, 0.25};
        registry->get<Transform>(bulletID).matrixOutdated = true;
    }
    else {
        ai.state = TowerStates::IDLE;
    }
}

/**
 * @brief AIsystem::eventHandler, event handling
 */
void AISystem::eventHandler(AIComponent &ai)
{
    while (!ai.notification_queue.empty()) {
        auto event = ai.notification_queue.front();
        switch (event) {
        case NPCevents::ENDPOINT_ARRIVED:
            ai.state = NPCstates::GOAL_REACHED;
            break;
        case NPCevents::DAMAGE_TAKEN:
            if (ai.health <= 0) {
                ai.state = NPCstates::DEATH;
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
        return NPCevents::ENDPOINT_ARRIVED;
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
