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
}

void MeshComponent::setShader(Shader *shader) {
    mShader = shader;
}

void MeshComponent::copyOpenGLData(const MeshComponent &other) {
    mVAO = other.mVAO;
    mVBO = other.mVBO;
    mEAB = other.mEAB; //holds the indices (Element Array Buffer - EAB)

    mVerticeCount = other.mVerticeCount;
    mIndiceCount = other.mIndiceCount;
    mDrawType = other.mDrawType;
}
