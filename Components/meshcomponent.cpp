#include "meshcomponent.h"
#include "innpch.h"
#include "materialcomponent.h"
MeshComponent::MeshComponent() {
    mType = CType::Mesh;
}

MeshComponent::~MeshComponent() {
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void MeshComponent::update(float dt) {
    glUseProgram(mShader->getProgram());
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, mIndiceCount, GL_UNSIGNED_INT, nullptr);
}

void MeshComponent::setShader(Shader *shader) {
    mShader = shader;
}
