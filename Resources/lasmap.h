#ifndef LASMAP_H
#define LASMAP_H

#include "LASLoader.h"
#include "vector3d.h"
class LasMap {
public:
    LasMap();
    ~LasMap();

    gsl::LASLoader lasloader{/*"../VSIMOblig/LASdata/33-1-497-327-20.las"*/};

    void printSomePoints();
    void addAllPointsToVertices();

    void normalizePoints();

    std::vector<gsl::Vector3D> mapToGrid(const std::vector<gsl::Vector3D> &points, int xGrid, int zGrid, gsl::Vector3D min, gsl::Vector3D max);

    void constructSurface(unsigned int xGridSize, unsigned int zGridSize);
    std::vector<gsl::Vector3D> points;
    std::vector<gsl::Vector3D> planePoints;
    //"../VSIMOblig/LASdata/33-1-497-327-20.las"

    float xMin;
    float yMin;
    float zMin;

    float xMax;
    float yMax;
    float zMax;

    float scaleFactor = 10;
    float length(const gsl::Vector3D &a, const gsl::Vector3D &b);
};

#endif // LASMAP_H
