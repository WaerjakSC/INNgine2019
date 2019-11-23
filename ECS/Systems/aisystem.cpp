#include "aisystem.h"
#include "components.h"
#include "gsl_math.h"
#include "movementsystem.h"
#include "registry.h"
AISystem::AISystem() {
}

/**
 * @brief AIsystem::update, runs the highly sofisticated AI cortex
 * @param dt
 */
void AISystem::update(DeltaTime dt) {
    // Run the eventHandler incase of events
    eventHandler();
    // draw the bspline curve lines
    draw();
    // Currently only set up for one entity
    std::optional<NPCevents> event;

    switch (state) {
    case MOVE:
        event = move(dt * 0.1f);
        if (event) {
            notification_queue.push(event.value());
        }
        break;
    case LEARN:
        learn();
        break;
    case DEATH:
        // Whatever happens when gnomes die
        death();
        break;
    case GOAL_REACHED:
        // implement this for folder
        goalReached();
        break;
    }
}

/**
 * @brief AIsystem::learn, flips the direction and updates path
 */
void AISystem::learn() {
    dir = -dir;
    if (updatePath) {
        mCurve.updatePath();
        //            mCurve.updateTrophies();
        updatePath = false;
    }
    state = MOVE;
}

/**
 * @brief AIsystem::masterOfCurves, helper function that updates trophies and path
 */
void AISystem::masterOfCurves() {
    mCurve.updateTrophies();
    mCurve.updatePath();
}
void AISystem::setHealth(int health) {
    auto ai = registry->get<AIComponent>(registry->getSelectedEntity());
    ai.hp = health;
}
void AISystem::setBSPlinePointX(double xIn) {
    GLuint entityID = registry->getSelectedEntity();
    auto &bspline = registry->get<BSplinePoint>(entityID);
    bspline.location.x = xIn;
    masterOfCurves();
}

void AISystem::setBSPlinePointY(double yIn) {
    GLuint entityID = registry->getSelectedEntity();
    auto &bspline = registry->get<BSplinePoint>(entityID);
    bspline.location.y = yIn;
    masterOfCurves();
}

void AISystem::setBSPlinePointZ(double zIn) {
    GLuint entityID = registry->getSelectedEntity();
    auto &bspline = registry->get<BSplinePoint>(entityID);
    bspline.location.z = zIn;
    masterOfCurves();
}
/**
 * @brief AIsystem::setControlPoints
 * @param cps
 */
void AISystem::setControlPoints(std::vector<vec3> cps) {
    mCurve.setControlPoints(cps);
}

/**
 * @brief AIsystem::eventHandler, event handling
 */
void AISystem::eventHandler() {
    auto reg = Registry::instance();
    auto view = reg->view<Transform, AIComponent>();
    auto &ai = view.get<AIComponent>(NPC);

    while (!notification_queue.empty()) {
        auto event = notification_queue.front();
        switch (event) {
        case ENDPOINT_ARRIVED:
            state = LEARN;
            break;
        case ITEM_TAKEN:
            // state = CRY
            updatePath = true;
            break;
        case DAMAGE_TAKEN:
            // something
            if (ai.hp <= 0) {
                state = DEATH;
            }
            break;
        }
        notification_queue.pop();
    }
}

/**
 * @brief AIsystem::draw
 */
void AISystem::draw() {
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
 * @brief AIsystem::move, patrols the NPC along the curve
 * @param deltaT
 * @return
 */
std::optional<NPCevents> AISystem::move(float deltaT) {
    auto reg = Registry::instance();
    auto view = reg->view<Transform, AIComponent>();
    auto &transform = view.get<Transform>(NPC);
    t += deltaT * dir;
    bool endPoint = 0.98f <= t || t < 0.f;

    if (endPoint)
        t = gsl::clamp(t, 0.f, 1.f);

    auto p = mCurve.eval(t);
    transform.localPosition = p;
    transform.matrixOutdated = true;

    if (endPoint) {
        // remove 1 hp from player
        return ENDPOINT_ARRIVED;
    }

    return std::nullopt;
}

/**
 * @brief AIsystem::init, initializes NPC and curve
 * @param eID
 */
void AISystem::init(GLuint eID) {
    NPC = eID;
    mCurve.init();
}

/**
 * @brief AIsystem::death, todo
 */
void AISystem::death() {
    // hp <= 0
    // gold++
    // delete entity
}

/**
 * @brief AIsystem::goalReached, todo
 */
void AISystem::goalReached() {
    // endpoint reached
    // remove 1LP from player
    // delete entity
}
