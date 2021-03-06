#include "aisystem.h"
#include "gsl_math.h"
#include "registry.h"
#include "resourcemanager.h"
#include "hud.h"

AISystem::AISystem() : registry(Registry::instance())
{
}

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
        case TowerStates::PLACEMENT:
            break;
        }
    }
    bulletLifeTime(dt);
}

void AISystem::bulletLifeTime(DeltaTime dt)
{
    auto view{registry->view<Bullet>()};
    for (auto entity : view) {
        auto &bullet{view.get(entity)};
        if (bullet.lifeTime >= 0.f)
            bullet.lifeTime -= dt;
        if (bullet.lifeTime <= 0.f) {
            registry->removeEntity(entity);
        }
    }
}
void AISystem::updateEditorOnly(DeltaTime)
{
    draw();
}

void AISystem::reset()
{
    curWaveCD = 1.f;
    curSpawnCD = 0.f;
    curSpawnDuration = 0.f;
    curTimerCD = 2.0f;
    curTimer = 0.f;
    auto &player{registry->getPlayer()};
    player.gold = player.initialGold;
    player.health = player.initialHealth;
}

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
        for (auto entity : sphere.overlappedEntities.getList()) {
            if (entity != ai.lastTarget) {
                ai.lastTarget = ai.targetID;
                ai.targetID = entity;
            }
        }
        ai.state = TowerStates::ATTACK;
    }
    else {
        ai.state = TowerStates::IDLE;
    }
}

void AISystem::attack(TowerComponent &tower, Transform &t)
{
    // Splash/Rocket type
    // BSpline from Tower to Enemy entity,
    // 2 Controlpoints -> First control point is offset by some value on y axis, 2nd is set at target location

    // Standard/Projectile type
    if (registry->contains<Transform>(tower.targetID) && tower.targetID != tower.lastTarget) {
        auto &trans{registry->get<Transform>(tower.targetID)};
        GLuint bulletID = ResourceManager::instance()->makeOctBall("projectile", 1);
        vec3 velocity{(trans.localPosition - t.localPosition).normalized()}; // get the vector (line) from tower to enemy, normalize to get the general direction.
        registry->add<Bullet>(bulletID, velocity, tower.damage, tower.projectileSpeed);
        registry->add<Sphere>(bulletID, vec3{0}, .25f, false);
        registry->get<Transform>(bulletID).localPosition = t.position;
        registry->get<Transform>(bulletID).localScale = vec3{0.25, 0.25, 0.25};
        registry->get<Transform>(bulletID).matrixOutdated = true;
    }
    else {
        tower.state = TowerStates::IDLE;
    }
}

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

void AISystem::draw()
{
    mCurve.draw();
}

void AISystem::deathTimer(DeltaTime dt)
{
    for (auto entity : deadAI) {
        registry->get<AIComponent>(entity);
        if (curTimerCD >= 0.f)
            curTimerCD -= dt;
        if (curTimerCD <= 0.f) {
            registry->removeEntity(entity);
            curTimerCD = curTimer;
        }
    }
}

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
        return NPCevents::ENDPOINT_ARRIVED;
    }

    return {};
}

void AISystem::init()
{
    mCurve.init();
    masterOfCurves();
}

void AISystem::death(const GLuint entityID)
{
    registry->getPlayer().gold += 20;
    registry->removeEntity(entityID);
    qDebug() << "Murdered another innocent gnome!";
}

void AISystem::goalReached(const GLuint entityID)
{
    HUD hud;
    registry->getPlayer().health--;
    registry->removeEntity(entityID);   
    hud.updatehealth();
}

void AISystem::masterOfCurves()
{
    mCurve.updateControlPoints();
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

void AISystem::setControlPoints(std::vector<vec3> cps)
{
    mCurve.setControlPoints(cps);
}
