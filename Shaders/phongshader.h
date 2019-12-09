#ifndef PHONGSHADER_H
#define PHONGSHADER_H

#include "shader.h"

struct LightData;
struct Light;
class PhongShader : public Shader {
public:
    PhongShader(cjk::Ref<CameraController> camController = nullptr, const GLchar *geometryPath = nullptr);
    virtual ~PhongShader() override;

    void transmitUniformData(gsl::Matrix4x4 &modelMatrix, Material *material) override;

    void setLight(GLuint entt);

private:
    //    GLint textureUniform{-1};
    GLint mLightColorUniform{-1};
    GLint mObjectColorUniform{-1};
    GLint mAmbientLightStrengthUniform{-1};
    GLint mAmbientColorUniform{-1};
    GLint mLightPositionUniform{-1};
    GLint mCameraPositionUniform{-1};
    GLint mSpecularStrengthUniform{-1};
    GLint mSpecularExponentUniform{-1};
    GLint mLightPowerUniform{-1};
    GLint textureUniform{-1};

    GLuint mLightID;
};



#endif // PHONGSHADER_H
