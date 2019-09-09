#include "meshcomponent.h"
#include "innpch.h"
#include "materialcomponent.h"
MeshComponent::MeshComponent() {
    mType = CType::Mesh;
}

MeshComponent::~MeshComponent() {
    //    if (mVAO != 0)
    //        glDeleteVertexArrays(1, &mVAO);
    //    if (mVBO != 0)
    //        glDeleteBuffers(1, &mVBO);
}

void MeshComponent::update(float dt) {
    //    glUseProgram(mShader->getProgram());
    //    glBindVertexArray(mVAO);
    //    if (mIndiceCount > 0)
    //        glDrawElements(mDrawType, mIndiceCount, GL_UNSIGNED_INT, nullptr);
    //    else
    //        glDrawArrays(mDrawType, 0, mVerticeCount);
}

void MeshComponent::setShader(Shader *shader) {
    mShader = shader;
}
