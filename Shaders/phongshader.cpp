
#include "phongshader.h"
#include "innpch.h"
#include "light.h"

PhongShader::PhongShader(const std::string shaderName, const GLchar *geometryPath)
    : Shader(shaderName, geometryPath) {
    mMatrixUniform = glGetUniformLocation(program, "mMatrix");
    vMatrixUniform = glGetUniformLocation(program, "vMatrix");
    pMatrixUniform = glGetUniformLocation(program, "pMatrix");

    mLightColorUniform = glGetUniformLocation(program, "lightColor");
    mObjectColorUniform = glGetUniformLocation(program, "objectColor");
    mAmbientLightStrengthUniform = glGetUniformLocation(program, "ambientStrength");
    mLightPositionUniform = glGetUniformLocation(program, "lightPosition");
    mSpecularStrengthUniform = glGetUniformLocation(program, "specularStrength");
    mSpecularExponentUniform = glGetUniformLocation(program, "specularExponent");
    mLightPowerUniform = glGetUniformLocation(program, "lightPower");
    mCameraPositionUniform = glGetUniformLocation(program, "cameraPosition");
}

PhongShader::~PhongShader() {
    qDebug() << "Deleting PhongShader";
}

void PhongShader::updateLightUniforms(gsl::Matrix4x4 *modelMatrix, const LightData &light) {
    glUniform1f(mAmbientLightStrengthUniform, light.mAmbientStrength);
    glUniform1f(mLightPowerUniform, light.mLightStrength);
    glUniform3f(mLightColorUniform, light.mLightColor.x, light.mLightColor.y, light.mLightColor.z);
    glUniform3f(mLightPositionUniform, modelMatrix->getPosition().x, modelMatrix->getPosition().y, modelMatrix->getPosition().z);
    glUniform3f(mObjectColorUniform, light.mObjectColor.x, light.mObjectColor.y, light.mObjectColor.z);
}
