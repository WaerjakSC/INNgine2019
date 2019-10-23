#include "aisystem.h"
#include "registry.h"

AIsystem::AIsystem()
{
    reg->registerComponent<BSplineCurve>();
}

void AIsystem::draw(GLint positionAttribute, GLint colorAttribute, GLint textureAttribute)
{

}

void AIsystem::setKnotsAndControlPoints(std::vector<float> knots, std::vector<vec3> points)
{

}

vec3 AIsystem::evaluateBSpline(int my, float x)
{

}

int AIsystem::findKnotInterval(float x)
{

}

vec3 AIsystem::evaluateBSpline(int degree, int startKnot, float x)
{

}


