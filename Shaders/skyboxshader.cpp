#include "skyboxshader.h"
#include "cameracontroller.h"

SkyboxShader::SkyboxShader(cjk::Ref<CameraController> camController, const GLchar *geometryPath)
    : Shader{camController, "SkyboxShader", geometryPath}
{
    vMatrixUniform = glGetUniformLocation(program, "vMatrix");
    pMatrixUniform = glGetUniformLocation(program, "pMatrix");

    skyboxTexUniform = glGetUniformLocation(program, "skybox");
}
void SkyboxShader::transmitUniformData(gsl::Matrix4x4 &modelMatrix, Material *material)
{
    Q_UNUSED(modelMatrix);
    gsl::Matrix4x4 view = mCameraController->getCamera().mViewMatrix.toMatrix3().toMatrix4(); // Convert to matrix3 and back again to remove translations
    glUniformMatrix4fv(vMatrixUniform, 1, GL_TRUE, view.constData());
    glUniformMatrix4fv(pMatrixUniform, 1, GL_TRUE, mCameraController->getCamera().mProjectionMatrix.constData());

    glUniform1i(skyboxTexUniform, material->mTextureUnit); //TextureUnit = 0 as default);
}


