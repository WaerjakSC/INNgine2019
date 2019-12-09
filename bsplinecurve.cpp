#include "bsplinecurve.h"
#include "registry.h"

BSplineCurve::BSplineCurve(int degree) : d{degree}
{
    debugShader = ResourceManager::instance()->getShader<ColorShader>();
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

    auto startEndSize{static_cast<unsigned int>(d + 1)};
    auto n{b.size() + startEndSize};

    if (n < 2 * startEndSize)
        return {};

    auto innerSize{n - 2 * startEndSize};

    std::vector<float> t;
    t.reserve(n);

    for (unsigned int i{0}; i < n; ++i) {
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
    for (unsigned int i{0}; !t.empty() && i < t.size() - 1; ++i)
        if (t[i] <= x && x < t[i + 1])
            return static_cast<int>(i);

    return -1;
}

/**
 * @brief updateTrophies call this when a trophy is taken
 */
void BSplineCurve::updateTrophies()
{
    std::vector<vec3> controlPoints;
    auto view{Registry::instance()->view<BSplinePoint>()}; // Get every entity with these two components
    for (auto entity : view) {
        auto &bspline{view.get(entity)};
        controlPoints.push_back(bspline.location);
    }

    setControlPoints(controlPoints);
}

/**
 * @brief BSplineCurve::updatePath, updates the bspline, called from AIsystem.
 */
void BSplineCurve::updatePath(bool init)
{
    std::vector<Vertex> vertices;

    vertices.reserve(splineResolution + b.size());
    std::vector<vec3> localPos;
    auto view{Registry::instance()->view<Transform, BSplinePoint>()}; // Get every entity with these two components
    for (int i{0}; i < splineResolution; ++i) {
        auto p{eval(i * 1.f / splineResolution)};
        vertices.emplace_back(p.x, p.y, p.z, 0.f, 1.f, 0.f);
        if (i == 0 || i == 15 || i == 30 || i == 49) {
            localPos.push_back(p);
        }
    }
    if (init) {
        std::vector<Transform *> temp;
        for (auto entity : view) {
            auto &bspline{view.get<Transform>(entity)};
            temp.push_back(&bspline);
        }
        for (size_t i{0}; i < temp.size(); i++) {
            temp[i]->localPosition = localPos[i];
            temp[i]->localPosition.y += 0.5f;
            temp[i]->matrixOutdated = true;
        }
    }
    // Control points
    for (size_t i{0}; i < b.size(); ++i) {
        auto p{b.at(i)};
        vertices.emplace_back(Vertex{p.x, p.y, p.z, 1.f, 0.f, 0.f});
    }

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

/**
 * @brief BSplineCurve::evaluateBSpline, deBoor's algorithm for bsplines
 * @param my et tall slik at bspline.t[my] <= x < bspline.t[my+1]
 * @param x parameterverdi på skjøtvektor
 * @return et punkt på splinekurven
 */
gsl::Vector3D BSplineCurve::evaluateBSpline(int my, float x) const
{
    std::vector<vec3> a;
    a.resize(t.size() + d + 1);

    for (int j{0}; j <= d; j++) {
        a[d - j] = b[my - j];
    }

    for (int k{d}; k > 0; k--) {
        int j{my - k};
        for (int i{0}; i < k; i++) {
            j++;
            float w{(x - t[j]) / (t[j + k] - t[j])};
            a[i] = a[i] * (1 - w) + a[i + 1] * w;
        }
    }
    return a[0];
}

/**
 * @brief BSplineCurve::draw
 */
void BSplineCurve::draw()
{
    if (debugLine) {
        glUseProgram(debugShader->getProgram());
        glPointSize(3.f);
        glBindVertexArray(mVAO);
        glDrawArrays(GL_LINE_STRIP, 0, splineResolution);
        glDrawArrays(GL_POINTS, splineResolution, b.size());
    }
}

/**
 * @brief BSplineCurve::init
 */
void BSplineCurve::init()
{
    initializeOpenGLFunctions();
    updateTrophies();

    // Spline curve
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (GLvoid *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    updatePath(true);
}

/**
 * @brief BSplineCurve::eval
 * @param x
 * @return
 */
gsl::Vector3D BSplineCurve::eval(float x) const
{
    auto my{getMy(x)};
    if (my > -1)
        return evaluateBSpline(my, x);

    return {0.f, 0.f, 0.f};
}
