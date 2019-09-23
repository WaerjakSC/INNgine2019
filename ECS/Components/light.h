#ifndef LIGHTINGCOMPONENT_H
#define LIGHTINGCOMPONENT_H

#include "gsl_math.h"
#include "material.h"

struct LightData {
    GLfloat mAmbientStrength{0.3f};
    gsl::Vector3D mAmbientColor{0.3f, 0.3f, 0.3f};

    GLfloat mLightStrength{0.7f};
    gsl::Vector3D mLightColor{0.3f, 0.3f, 0.3f};

    GLfloat mSpecularStrength{0.3f};
    GLint mSpecularExponent{4};
    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
};

/**
 * @brief The LightingComponent class holds relevant info for objects that should light the environment
 */
class Light : public Component {
public:
    Light(LightData data = LightData());
    virtual void update(float dt);

    //    void draw(gsl::Matrix4x4 &mMatrix);

    //    void setColor(const gsl::Vector3D &color);

    LightData getLight() const;

    void setLightColor(const gsl::Vector3D &color);

private:
    LightData light;
};

#endif // LIGHTINGCOMPONENT_H
