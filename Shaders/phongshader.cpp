
#include "phongshader.h"
#include "camera.h"
#include "components.h"
#include "innpch.h"
#include "registry.h"

PhongShader::PhongShader(const GLchar *geometryPath)
    : Shader("PhongShader", geometryPath) {
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
    Light &light = Registry::instance()->getComponent<Light>(mLight->id());
    Transform &lightTrans = Registry::instance()->getComponent<Transform>(mLight->id());

    glUniform1i(textureUniform, material->mTextureUnit); //TextureUnit = 0 as default);
    glUniform1f(mAmbientLightStrengthUniform, light.mAmbientStrength);
    glUniform3f(mAmbientColorUniform, light.mAmbientColor.x, light.mAmbientColor.y, light.mAmbientColor.z);
    glUniform1f(mLightPowerUniform, light.mLightStrength);
    glUniform3f(mLightColorUniform, light.mLightColor.x, light.mLightColor.y, light.mLightColor.z);
    glUniform3f(mLightPositionUniform, lightTrans.position.x, lightTrans.position.y, lightTrans.position.z);
    glUniform1i(mSpecularExponentUniform, material->mSpecularExponent);
    glUniform1f(mSpecularStrengthUniform, material->mSpecularStrength);
    glUniform3f(mObjectColorUniform, material->mObjectColor.x, material->mObjectColor.y, material->mObjectColor.z);
    glUniform3f(mCameraPositionUniform, mCurrentCamera->position().x, mCurrentCamera->position().y, mCurrentCamera->position().z);
}
void PhongShader::setLight(Entity *entt) {
    mLight = entt;
}
