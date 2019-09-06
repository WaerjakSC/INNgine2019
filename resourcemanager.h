#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Components/meshcomponent.h"
#include "Shaders/shader.h"
#include "gameobject.h"
#include "texture.h"
#include <QOpenGLFunctions_4_1_Core>

#include "Components/comppch.h"
#include "Shaders/shader.h"
#include "texture.h"

// Resource manager work in progress
struct meshData {
    meshData() = default;
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
    void Clear() {
        mVertices.clear();
        mIndices.clear();
    }
};
class ResourceManager : public QOpenGLFunctions_4_1_Core {
public:
    static ResourceManager &instance();
    virtual ~ResourceManager();

    // Loads and generates shader (takes name and the shader)
    void LoadShader(ShaderType type, const GLchar *geometryPath = nullptr);
    // Gets stored shader
    Shader *GetShader(ShaderType type);
    // Loads and generates texture from file
    void LoadTexture(std::string name, GLuint textureUnit = 0);
    // Gets stored texture
    Texture *GetTexture(std::string name);
    // Gets the mesh
    MeshComponent *GetMesh(std::string name);
    // De-allocate loaded resources from memory
    void Clear();

    GLuint makeGameObject(std::string name = "");

    void addInputComponent(MainWindow *mainWindow, int eID = -1);
    void addComponent(CType type, int eID = -1);
    Component *getComponent(CType type, int eID = -1);
    void addMeshComponent(std::string name, int eID = -1);

    // Basic Shapes and Prefabs
    GLuint makeXYZ();
    GLuint makeSkyBox();
    GLuint makeTriangleSurface(std::string fileName);
    GLuint makeBillBoard();
    GLuint makeOctBall(int n);
    GLuint makeLightObject();

private:
    // Private constructor
    ResourceManager();
    GLuint mNumObjects{0};
    // std::map(key, object) for easy resource storage
    std::map<ShaderType, Shader *> Shaders;
    std::map<std::string, Texture *> Textures;
    std::map<std::string, unsigned int> mMeshMap;

    // Temp mVertices/mIndices container. Cleared before each use.
    meshData mMesh;

    // Component vectors
    std::vector<InputComponent> mInputs;
    std::vector<LightingComponent> mLighting;
    std::vector<MaterialComponent> mMaterials;
    std::vector<MeshComponent> mMeshes;
    std::vector<PhysicsComponent> mPhysics;
    std::vector<SoundComponent> mSounds;
    std::vector<TransformComponent> mTransforms;
    std::vector<GameObject> mGameObjects;

    // OpenGL init functions
    void initVertexBuffers();
    void initIndexBuffers();

    // Reads and loads mesh
    MeshComponent *LoadMesh(std::string fileName);
    void setMesh(MeshComponent *mesh, int eID);
    bool readFile(std::string fileName);

    bool readTriangleFile(std::string filename);
    MeshComponent *LoadTriangleMesh(std::string fileName);

    // OctahedronBall functions
    void makeUnitOctahedron(GLint recursions);
    void subDivide(const gsl::Vector3D &a, const gsl::Vector3D &b, const gsl::Vector3D &c, GLint n);
    void makeTriangle(const gsl::Vector3D &v1, const gsl::Vector3D &v2, const gsl::Vector3D &v3);
};

#endif // RESOURCEMANAGER_H
