#include "meshcomponent.h"
#include "innpch.h"

MeshComponent::MeshComponent() {
}

MeshComponent::MeshComponent(meshData *mesh) : mMesh(mesh) {
}

MeshComponent::~MeshComponent() {
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void MeshComponent::init() {
    if (mMesh) { // Make sure you're not trying to use a nullptr
        //must call this to use OpenGL functions
        initializeOpenGLFunctions();

        //Vertex Array Object - VAO
        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);

        //Vertex Buffer Object to hold vertices - VBO
        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        glBufferData(GL_ARRAY_BUFFER, mMesh->mVertices.size() * sizeof(Vertex), mMesh->mVertices.data(), GL_STATIC_DRAW);

        // 1rst attribute buffer : vertices
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
        glEnableVertexAttribArray(0);

        // 2nd attribute buffer : colors
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        // 3rd attribute buffer : uvs
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

        //Second buffer - holds the indices (Element Array Buffer - EAB):
        glGenBuffers(1, &mEAB);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEAB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mMesh->mIndices.size() * sizeof(GLuint), mMesh->mIndices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }
}

void MeshComponent::draw(gsl::Matrix4x4 &mMatrix) {
    glUseProgram(mMaterial.mShader->getProgram());
    glBindVertexArray(mVAO);
    mMaterial.mShader->transmitUniformData(&mMatrix, &mMaterial);
    if (mMesh) {
        glDrawElements(GL_TRIANGLES, mMesh->mIndices.size(), GL_UNSIGNED_INT, nullptr);
    }
    //    glBindVertexArray(0);
}
void MeshComponent::setShader(Shader *shader) {
    mMaterial.mShader = shader;
}

meshData *MeshComponent::getMesh() const {
    return mMesh;
}

void MeshComponent::setMesh(meshData *mesh) {
    mMesh = mesh;
}
