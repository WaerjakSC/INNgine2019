#include "aisystem.h"
#include "components.h"
#include "registry.h"

AIsystem::AIsystem() {
    Registry::instance()->registerComponent<BSplineCurve>();
}

void AIsystem::update(float deltaTime) {
    // RUN FSM HERE?
    switch (state) {

    case MOVE:
        move();
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

void AIsystem::draw(GLint positionAttribute, GLint colorAttribute, GLint textureAttribute) {
}

void AIsystem::setKnotsAndControlPoints(std::vector<float> knots, std::vector<vec3> points) {
}

/**
 * @brief AIsystem::evaluateBSpline, deBoor's algorithm for b-splines
 * @param bspline referanse til en bsplinecurve
 * @param my et tall slik at bspline.t[my] <= x < bspline.t[my+1]
 * @param x paramterverdi på skjøtvektor
 * @return et punkt på splinekurven
 */
vec3 AIsystem::evaluateBSpline(const BSplineCurve &bspline, int my, float x) {
    vec3 a[20];
    int d = bspline.d;

    for (int j = 0; j <= d; j++) {
        a[d - j] = bspline.b[my - j];
    }

    for (int k = d; k > 0; k--) {
        int j = my - k;
        for (int i = 0; i < k; i++) {
            j++;
            float w = (x - bspline.t[j]) / (bspline.t[j + k] - bspline.t[j]);
            a[i] = a[i] * (i - w) + a[i + 1] * w;
        }
    }
    return a[0];
}

int AIsystem::findKnotInterval(float x) {
}

vec3 AIsystem::evaluateBSpline(int degree, int startKnot, float x) {
}

vec2 AIsystem::deBoor(float x)
{
    // return curve position calculated by deBoor's algorithm (evaluateBSpline)
}

void AIsystem::move()
{

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


}

void AIsystem::death(){

    // hp >= 0
    // gold++
    // delete entity

}

void AIsystem::goalReached()
{
    // endpoint reached
    // remove 1LP from player
    // delete entity
}


