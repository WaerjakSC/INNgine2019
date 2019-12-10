#include "particleshader.h"
#include "cameracontroller.h"

ParticleShader::ParticleShader(cjk::Ref<CameraController> camController, const GLchar *geometryPath)
    : Shader{camController, "ParticleShader", geometryPath}
{
    vMatrixUniform = glGetUniformLocation(program, "vMatrix");
    pMatrixUniform = glGetUniformLocation(program, "pMatrix");

    textureUniform = glGetUniformLocation(program, "textureSampler");
    cameraRightUniform = glGetUniformLocation(program, "cameraRight");
    cameraUpUniform = glGetUniformLocation(program, "cameraUp");
}
void ParticleShader::transmitParticleUniformData(const ParticleEmitter &emitter)
{
    glUniformMatrix4fv(vMatrixUniform, 1, GL_TRUE, mCameraController->getCamera().mViewMatrix.constData());
    glUniformMatrix4fv(pMatrixUniform, 1, GL_TRUE, mCameraController->getCamera().mProjectionMatrix.constData());

    glUniform1i(textureUniform, emitter.textureUnit); //TextureUnit = 0 as default);
    const vec3 &right = mCameraController->right();
    glUniform3f(cameraRightUniform, right.x, right.y, right.z);
    const vec3 &up = mCameraController->up();
    glUniform3f(cameraUpUniform, up.x, up.y, up.z);
}
