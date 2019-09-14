#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Components/comppch.h"
#include "Shaders/shader.h"
#include "pool.h"
#include "texture.h"
#include <QOpenGLFunctions_4_1_Core>

class RenderView;
class GameObject;
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

    void setMainWindow(MainWindow *window) { mMainWindow = window; }

    GLuint makeGameObject(std::string name = "");

    void addComponent(CType type, int eID = -1);
    void removeComponent(CType type, int eID);
    Component *getComponent(CType type, int eID = -1);
    void addMeshComponent(std::string name, int eID = -1);

    // Basic Shapes and Prefabs
    GLuint makeXYZ();
    GLuint makeSkyBox();
    GLuint makeTriangleSurface(std::string fileName);
    GLuint makeBillBoard();
    GLuint makeOctBall(int n);
    GLuint makeLightObject();
    GLuint make3DObject(std::string name, ShaderType type = Phong);

    RenderView *getRenderView() const;

    std::map<ShaderType, Shader *> getShaders() const;

    void setMesh(std::string name, int eID);

    GLuint makePlane();
    GLuint makeCube();

    std::vector<GameObject *> getGameObjects() const;

    Pool<TransformComponent> &getTransforms();

    GameObject *getGameObject(int eID);
    void removeGameObject(int eID);

    std::vector<int> getGameObjectIndex() const;

    void setParent(int eID, int parentID);

private:
    // Private constructor
    ResourceManager();
    GLuint mNumGameObjects{0};
    // std::map(key, object) for easy resource storage
    std::map<ShaderType, Shader *> Shaders;
    std::map<std::string, Texture *> Textures;
    std::map<std::string, MeshComponent> mMeshMap; // Holds each unique mesh for easy access

    // Temp mVertices/mIndices container. Cleared before each use.
    meshData mMeshData;

    MainWindow *mMainWindow;
    std::vector<int> mGameObjectIndex;      // Holds the sparse array for gameobjects.
    std::vector<GameObject *> mGameObjects; // Save GameObjects as pointers to avoid clipping of derived classes

    // Component pools
    Pool<MeshComponent> mMeshes;
    Pool<MaterialComponent> mMaterials;
    Pool<TransformComponent> mTransforms;
    Pool<LightingComponent> mLights;
    Pool<InputComponent> mInputs;
    Pool<PhysicsComponent> mPhysics;
    Pool<SoundComponent> mSounds;
    // View class for collecting the components rendersystem needs
    RenderView *mRenderView; // TODO: Give RenderView Material and MeshComponent pools

    // OpenGL init functions
    void initVertexBuffers(MeshComponent *mesh);
    void initIndexBuffers(MeshComponent *mesh);

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
