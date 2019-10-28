#include "aisystem.h"
#include "components.h"
#include "registry.h"

AIsystem::AIsystem(){
    Registry::instance()->registerComponent<BSplineCurve>();
}

void AIsystem::draw(GLint positionAttribute, GLint colorAttribute, GLint textureAttribute)
{

}

void AIsystem::setKnotsAndControlPoints(std::vector<float> knots, std::vector<vec3> points)
{

}

/**
 * @brief AIsystem::evaluateBSpline
 * @param bspline referanse til en bsplinecurve
 * @param my et tall slik at bspline.t[my] <= x < bspline.t[my+1]
 * @param x paramterverdi på skjøtvektor
 * @return et punkt på splinekurven
 */
vec3 AIsystem::evaluateBSpline(const BSplineCurve &bspline, int my, float x)
{
    vec3 a[20];
    int d = bspline.d;

    for (int j=0; j<=d; j++){
        a[d-j] = bspline.b[my-j];
    }

    for(int k = d; k > 0; k--){
        int j = my-k;
        for(int i = 0; i < k; i++){
            j++;
            float w = (x-bspline.t[j])/(bspline.t[j+k] - bspline.t[j]);
            a[i] = a[i] * (i-w) + a[i+1] * w;
        }
    }
    return a[0];
}

int AIsystem::findKnotInterval(float x)
{

}

vec3 AIsystem::evaluateBSpline(int degree, int startKnot, float x)
{

}


