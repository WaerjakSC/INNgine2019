#include "aisystem.h"
#include "components.h"
#include "gsl_math.h"
#include "movementsystem.h"
#include "registry.h"
AIsystem::AIsystem() {
}

void AIsystem::update(DeltaTime dt) {
    // Run the eventHandler incase of events
    eventHandler();
    // draw the bspline curve lines
    draw();
    // Currently only set up for one entity
    std::optional<NPCevents> event;

    switch (state) {
    case MOVE:
        move(dt * 0.1f);
        if (event) {
            notification_queue.push(event.value());
        }
        break;
    case LEARN:
        dir = -dir;
        if (updatePath) {
            mCurve.updatePath();
            mCurve.updateTrophies();
            updatePath = false;
        }
        state = MOVE;
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

void AIsystem::setControlPoints(std::vector<vec3> cps) {
    mCurve.setControlPoints(cps);
}

void AIsystem::eventHandler() {
    auto reg = Registry::instance();
    auto view = reg->view<Transform, AIcomponent>();
    auto [transform, ai] = view.get<Transform, AIcomponent>(NPC);

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

void AIsystem::draw() {
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
std::optional<NPCevents> AIsystem::move(float deltaT) {
    auto reg = Registry::instance();
    auto view = reg->view<Transform, AIcomponent>();
    auto [transform, ai] = view.get<Transform, AIcomponent>(NPC);
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

void AIsystem::init(GLuint eID) {
    NPC = eID;
    mCurve.init();
}

void AIsystem::death() {
    // hp >= 0
    // gold++
    // delete entity
}

void AIsystem::goalReached() {
    // endpoint reached
    // remove 1LP from player
    // delete entity
}
