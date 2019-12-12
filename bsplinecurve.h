#ifndef BSPLINECURVE_H
#define BSPLINECURVE_H
#include "core.h"
#include "vector3d.h"
#include <QOpenGLFunctions_4_1_Core>

class ColorShader;
class BSplineCurve : protected QOpenGLFunctions_4_1_Core {
    using vec3 = gsl::Vector3D;

public:
    /** Default constructor for the bspline
     *
     * @param degree
     */
    BSplineCurve(int degree = 2);
    /// Control point for the bspline
    std::vector<vec3> b;
    /// Number of knots
    int n;
    /// Degrees
    int d;
    /// Knot vector
    std::vector<float> t;

    /** deBoor's algorithm for bsplines
     *
     * @param my A number so that bspline.t[my] <= x < bspline.t[my+1]
     * @param x knotvector value
     * @return A point on the splinecurve
     */
    vec3 evaluateBSpline(int my, float x) const;
    int splineResolution = 50.f;
    bool debugLine = true;
    void draw();
    void init();

    GLuint mVAO;
    GLuint mVBO;
    /** Finds the knots used by the bspline and stores them in a vector t.
     *
     * @return returns the knot vector t.
     */
    std::vector<float> findKnots() const;
    /** Sets the control points located in a given control point vector.
     *
     * @param cp
     */
    void setControlPoints(const std::vector<vec3> &cp);
    /** Calculates the my value used by deBoor's algorithm.
     *
     * @param x
     * @return returns the my value
     */
    int getMy(float x) const;
    /** Checks if the my value is non-negative.
     * If true, passes the my value to the evaluateBSpline function.
     * If false, returns 0.
     * @param x
     * @return
     */
    vec3 eval(float x) const;
    /** Updates and draws the Bspline curve.
     *  Iterates and places the bspline points according to spline resolution.
     *  Updates the bspline transform localPosition accordingly.
     * @param init
     */
    void updatePath(bool init = false);

    void updateControlPoints();

private:
    cjk::Ref<ColorShader> debugShader;
};

#endif // BSPLINECURVE_H
