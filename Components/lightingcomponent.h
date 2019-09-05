#ifndef LIGHTINGCOMPONENT_H
#define LIGHTINGCOMPONENT_H

#include "GSL/gsl_math.h"
#include "component.h"
/**
 * @brief The LightingComponent class holds relevant info for objects that should light the environment
 */
class LightingComponent : public Component {
public:
    LightingComponent();

    GLfloat mAmbientStrength{0.3f};
    gsl::Vector3D mAmbientColor{0.3f, 0.3f, 0.3f};

    GLfloat mLightStrength{0.7f};
    gsl::Vector3D mLightColor{0.3f, 0.3f, 0.3f};

    GLfloat mSpecularStrength{0.3f};
    GLint mSpecularExponent{4};
};

#endif // LIGHTINGCOMPONENT_H
