#include "meshcomponent.h"
#include "innpch.h"
#include "materialcomponent.h"
MeshComponent::MeshComponent() {
    mType = CType::Mesh;
}

MeshComponent::~MeshComponent() {
    //    glDeleteVertexArrays(1, &mVAO);
    //    glDeleteBuffers(1, &mVBO);
}

void MeshComponent::update(float dt) {
}

// Code initialization (Similar to Unity's void Start()), separating the constructor and code initialization
void MeshComponent::init() {
    //    if (mMesh) { // Make sure you're not trying to use a nullptr

    //    }
}

void MeshComponent::draw(MaterialComponent *material) {
    //    glUseProgram(material->getShader()->getProgram());
    //    glBindVertexArray(mVAO);
    //    if (mMesh) {
    //        glDrawElements(GL_TRIANGLES, mMesh->mIndices.size(), GL_UNSIGNED_INT, nullptr);
    //    }
}
