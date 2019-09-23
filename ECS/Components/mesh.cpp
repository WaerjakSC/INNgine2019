#include "mesh.h"
#include "innpch.h"
#include "material.h"
Mesh::Mesh() {
    mType = CType::Mesh;
}

Mesh::Mesh(GLenum drawType, GLuint verticeCount, GLuint indiceCount) : mVerticeCount(verticeCount), mIndiceCount(indiceCount), mDrawType(drawType) {
    mType = CType::Mesh;
}

Mesh::Mesh(GLenum drawType, meshData data) : Mesh(drawType, data.mVertices.size(), data.mIndices.size()) {
}

Mesh::~Mesh() {
    //    if (mVAO != 0)
    //        glDeleteVertexArrays(1, &mVAO);
    //    if (mVBO != 0)
    //        glDeleteBuffers(1, &mVBO);
}

void Mesh::update(float dt) {
}

void Mesh::copyOpenGLData(const Mesh &other) {
    mVAO = other.mVAO;
    mVBO = other.mVBO;
    mEAB = other.mEAB; //holds the indices (Element Array Buffer - EAB)

    mVerticeCount = other.mVerticeCount;
    mIndiceCount = other.mIndiceCount;
    mDrawType = other.mDrawType;
}
