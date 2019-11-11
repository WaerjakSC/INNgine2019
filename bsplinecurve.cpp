#include "bsplinecurve.h"


/**
 * @brief BSplineCurve::evaluateBSpline
 * @param bspline
 * @param my
 * @param x
 * @return
 */
vec3 BSplineCurve::evaluateBSpline(const BSplineCurve &bspline, int my, float x)
{

}

/**
 * @brief BSplineCurve::setControlPoints
 * @param cp
 */
void BSplineCurve::setControlPoints(const std::vector<vec3> &cp)
{
    std::size_t oldSize{b.size()};

    b = cp;

    if (cp.size() != oldSize)
        t = findKnots();

}

/**
 * @brief BSplineCurve::findKnots
 * @return
 */
std::vector<float> BSplineCurve::findKnots() const
{

    auto startEndSize = static_cast<unsigned int>(d + 1);
    auto n = b.size() + startEndSize;

    if (n < 2 * startEndSize)
        return {};

    auto innerSize = n - 2 * startEndSize;

    std::vector<float> t;
    t.reserve(n);

    for (unsigned int i{0}; i < n; ++i)
    {
        if (i < startEndSize)
            t.push_back(0.f);
        else if (n - startEndSize <= i)
            t.push_back(1.f);
        else
            t.push_back(static_cast<float>(i - startEndSize + 1) / (innerSize + 1));
    }

    return t;

}

/**
 * @brief BSplineCurve::getMy
 * @param x
 * @return
 */
int BSplineCurve::getMy(float x) const
{
    for (unsigned int i{0}; i < t.size() - 1; ++i)
        if (t[i] <= x && x < t[i + 1])
            return static_cast<int>(i);

    return -1;
}

/**
 * @brief BSplineCurve::evaluateBSpline
 * @param my
 * @param x
 * @return
 */
vec3 BSplineCurve::evaluateBSpline(int my, float x)
{
    std::vector<vec3> a;
    a.resize(t.size() + d + 1);

    for (int j = 0; j <= d; j++) {
        a[d - j] = b[my - j];
    }

    for (int k = d; k > 0; k--) {
        int j = my - k;
        for (int i = 0; i < k; i++) {
            j++;
            float w = (x - t[j]) / (t[j + k] - t[j]);
            a[i] = a[i] * (i - w) + a[i + 1] * w;
        }
    }
    return a[0];
}
