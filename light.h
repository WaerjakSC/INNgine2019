#ifndef LIGHT_H
#define LIGHT_H

#include "Components/meshcomponent.h"
#include "gameobject.h"

class Light : public GameObject {
public:
    Light(std::string name);

    GLfloat mAmbientStrenght{0.3f};
    gsl::Vector3D mAmbientColor{0.3f, 0.3f, 0.3f};

    GLfloat mLightStrenght{0.7f};
    gsl::Vector3D mLightColor{0.3f, 0.3f, 0.3f};

    GLfloat mSpecularStrenght{0.3f};
    GLint mSpecularExponent{4};
};

#endif // LIGHT_H
