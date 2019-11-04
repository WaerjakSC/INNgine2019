#include "lasmap.h"
#include "math_constants.h"
#include "vertex.h"
#include <QDebug>
#include <algorithm>
#include <cmath>

LasMap::LasMap() {
}

LasMap::~LasMap() {
}

void LasMap::printSomePoints() {
    for (auto point = lasloader.begin() + 10; point != lasloader.end() - 25; ++point) {
        std::cout << "Point: (" << point->xNorm() << ", " << point->yNorm() << ", " << point->zNorm() << ")" << std::endl;
    }
}

float LasMap::length(const gsl::Vector3D &a, const gsl::Vector3D &b) {
    return static_cast<float>(std::sqrt(std::pow(a.x + b.x, 2) + std::pow(a.y + b.y, 2) + std::pow(a.z + b.z, 2)));
}
