#include "visualobject.h"
#include "Shaders/shader.h"
#include "innpch.h"

VisualObject::VisualObject() {
}

VisualObject::~VisualObject() {
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void VisualObject::init() {
}

void VisualObject::setShader(Shader *shader) {
    mMaterial.mShader = shader;
}
