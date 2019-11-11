#ifndef BSPLINECURVE_H
#define BSPLINECURVE_H


class BSplineCurve
{
public:

    std::vector<vec3> b;  // control points
    int n;                // n number of knots
    int d;                // d degrees
    std::vector<float> t; // knots

    // void setKnotsAndControlPoints(std::vector<float> knots, std::vector<vec3> points);
    vec3 evaluateBSpline(const BSplineCurve &bspline, int my, float x);
    vec3 evaluateBSpline(int my, float x) const;

    int findKnotInterval(float x);
    std::vector<float> findKnots() const;
    void setControlPoints(const std::vector<vec3> &cp);

    int getMy(float x) const;
    vec3 eval(float x) const;


    //vec2 deBoor(float x);

    // default constructor
    BSplineCurve();
    BSplineCurve(std::vector<float> knots, std::vector<vec3> controlpoints, int degree = 2) : b(controlpoints), d(degree), t(knots) {
        n = knots.size();
    }


};

#endif // BSPLINECURVE_H
