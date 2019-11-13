
#include "phongshader.h"
#include "camera.h"
#include "cameracontroller.h"
#include "components.h"
#include "innpch.h"
#include "registry.h"

PhongShader::PhongShader(cjk::Ref<CameraController> camController, const GLchar *geometryPath)
    : Shader(camController, "PhongShader", geometryPath) {
    mMatrixUniform = glGetUniformLocation(program, "mMatrix");
    vMatrixUniform = glGetUniformLocation(program, "vMatrix");
    pMatrixUniform = glGetUniformLocation(program, "pMatrix");

    textureUniform = glGetUniformLocation(program, "textureSampler");
    mLightColorUniform = glGetUniformLocation(program, "lightColor");
    mObjectColorUniform = glGetUniformLocation(program, "objectColor");
    mAmbientLightStrengthUniform = glGetUniformLocation(program, "ambientStrength");
    mAmbientColorUniform = glGetUniformLocation(program, "ambientColor");
    mLightPositionUniform = glGetUniformLocation(program, "lightPosition");
    mSpecularStrengthUniform = glGetUniformLocation(program, "specularStrength");
    mSpecularExponentUniform = glGetUniformLocation(program, "specularExponent");
    mLightPowerUniform = glGetUniformLocation(program, "lightPower");
    mCameraPositionUniform = glGetUniformLocation(program, "cameraPosition");
}

PhongShader::~PhongShader() {
    qDebug() << "Deleting PhongShader";
}
void PhongShader::transmitUniformData(gsl::Matrix4x4 &modelMatrix, Material *material) {
    Shader::transmitUniformData(modelMatrix);
    auto view = Registry::instance()->view<Transform, Light>();
    auto [lightTrans, light] = view.get<Transform, Light>(mLight->id());

    glUniform1i(textureUniform, material->mTextureUnit - 1); //TextureUnit = 0 as default);
    glUniform1f(mAmbientLightStrengthUniform, light.mAmbientStrength);
    glUniform3f(mAmbientColorUniform, light.mAmbientColor.x, light.mAmbientColor.y, light.mAmbientColor.z);
    glUniform1f(mLightPowerUniform, light.mLightStrength);
    glUniform3f(mLightColorUniform, light.mLightColor.x, light.mLightColor.y, light.mLightColor.z);
    glUniform3f(mLightPositionUniform, lightTrans.position.x, lightTrans.position.y, lightTrans.position.z);
    glUniform1i(mSpecularExponentUniform, material->mSpecularExponent);
    glUniform1f(mSpecularStrengthUniform, material->mSpecularStrength);
    glUniform3f(mObjectColorUniform, material->mObjectColor.x, material->mObjectColor.y, material->mObjectColor.z);
    glUniform3f(mCameraPositionUniform, mCameraController->getCamera().position().x, mCameraController->getCamera().position().y, mCameraController->getCamera().position().z);
}
void PhongShader::setLight(cjk::Ref<Entity> entt) {
    mLight = entt;
}
