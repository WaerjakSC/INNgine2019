#include "aisystem.h"
#include "components.h"
#include "movementsystem.h"
#include "registry.h"

AIsystem::AIsystem() {
}

void AIsystem::update(DeltaTime dt) {
    // Run the eventHandler incase of events
    eventHandler();

    std::optional<NPCevents> event;

    switch (state) {
    case MOVE:
        move(dt*0.1f);
        if(event){
            notification_queue.push(event.value());
        }
        break;
    case DEATH:
        // Whatever happens when gnomes die
        death();
        break;
    case GOAL_REACHED:
        goalReached();
        break;
    }
}

void AIsystem::eventHandler(){
    auto reg = Registry::instance();
    auto view = reg->view<Transform, AIcomponent>();
    auto [ transform, ai ] = view.get<Transform, AIcomponent>(eID);

    while(!notification_queue.empty()){
        auto event = notification_queue.front();
        switch(event){
        case ENDPOINT_ARRIVED:
            state = GOAL_REACHED;
            break;
        case ITEM_TAKEN:
            // state = CRY
            // update path
            break;
        case DAMAGE_TAKEN:
            // something
            if (ai.hp <= 0){
                state = DEATH;
            }
            break;

        }
        notification_queue.pop();
    }
}

void AIsystem::draw(GLint positionAttribute, GLint colorAttribute, GLint textureAttribute) {
}


/**
 * @brief AIsystem::evaluateBSpline, deBoor's algorithm for b-splines
 * @param bspline referanse til en bsplinecurve
 * @param my et tall slik at bspline.t[my] <= x < bspline.t[my+1]
 * @param x paramterverdi på skjøtvektor
 * @return et punkt på splinekurven
 */

vec2 AIsystem::deBoor(float x) {
    // return curve position calculated by deBoor's algorithm (evaluateBSpline)
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
std::optional<NPCevents> AIsystem::move(float deltaT)
{
    auto reg = Registry::instance();
    auto view = reg->view<Transform, AIcomponent>();
    auto [ transform, ai ] = view.get<Transform, AIcomponent>(eID);
    t += deltaT * dir;
    bool endPoint = 0.98f <= t || t < 0.f;

    if (endPoint)
        t = gsl::clamp(t, 0.f, 1.f);

    auto p = rememberedCurve.eval(t);
    // Flytt til movementsystem senere
    transform.localPosition = p;
    transform.matrixOutdated = true;

    if (endPoint){
        // remove 1 hp from player
        return ENDPOINT_ARRIVED;
    }


    // move NPC

    return std::nullopt;


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
