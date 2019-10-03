#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "components.h"
#include "shader.h"
#include "texture.h"
#include <QOpenGLFunctions_4_1_Core>
#include <memory>

class RenderView;
class LightSystem;
class GameObject;
class Registry;
class MainWindow;

class ResourceManager : public QOpenGLFunctions_4_1_Core {
    friend class Scene;

public:
    static ResourceManager *instance();
    virtual ~ResourceManager();

    // Loads and generates shader (takes name and the shader)
    void loadShader(ShaderType type, const GLchar *geometryPath = nullptr);
    // Gets stored shader
    Shader *getShader(ShaderType type);
    // Loads and generates texture from file
    void loadTexture(std::string name);
    // Gets stored texture
    Texture *getTexture(std::string name);

    void setMainWindow(MainWindow *window) { mMainWindow = window; }

    void addMeshComponent(std::string name, int eID = -1);

    // Basic Shapes and Prefabs
    GLuint makeXYZ();
    GLuint makeSkyBox();
    GLuint makeTriangleSurface(std::string fileName, ShaderType type);
    GLuint makeBillBoard();
    GLuint makeOctBall(int n = 3);
    GLuint makePlane();
    GLuint makeCube();
    GLuint makeLightObject();
    GLuint make3DObject(std::string name, ShaderType type = Phong);

    std::map<ShaderType, Shader *> getShaders() const;

    void setMesh(std::string name, int eID);

    GLuint makeGameObject(std::string name = "");
    std::vector<GameObject *> getGameObjects() const;
    GameObject *getGameObject(int eID);
    void removeGameObject(int eID);
    void clearScene();

    //    RenderView *getRenderView() const;

    void setLightSystem(const std::shared_ptr<LightSystem> &lightSystem);

    std::vector<Component *> getComponents(int eID);

    QString getTextureName(GLuint id);

    QString getMeshName(const Mesh &mesh);

    void updateChildParent();
    void addBillBoard(GLuint entityID) { mBillBoards.push_back(entityID); }
    std::vector<int> billBoards() { return mBillBoards; }
    void setLoading(bool load) { mLoading = load; }

    bool isLoading() const;

private:
    // Private constructor
    ResourceManager();

    static ResourceManager *mInstance;
    Registry *registry;
    GLuint mNumGameObjects{0};
    bool mLoading{false};
    // std::map(key, object) for easy resource storage
    std::map<ShaderType, Shader *> mShaders;
    std::map<std::string, Texture *> mTextures;
    std::map<std::string, Mesh> mMeshMap; // Holds each unique mesh for easy access
    std::vector<int> mBillBoards;

    // Temp mVertices/mIndices container. Cleared before each use.
    meshData mMeshData;

    MainWindow *mMainWindow;
    //    std::vector<int> mGameObjectIndices;    // Holds the sparse array for gameobjects.
    std::vector<GameObject *> mGameObjects; // Save GameObjects as pointers to avoid clipping of derived classes
    // Systems
    std::shared_ptr<LightSystem> mLightSystem;

    // View class for collecting the components RenderSystem needs
    //    std::unique_ptr<RenderView> mRenderView; // Should this belong to RenderSystem?

    // OpenGL init functions
    void initVertexBuffers(Mesh *mesh);
    void initIndexBuffers(Mesh *mesh);
    void makeBillBoardMesh(int eID);

    // Reads and loads mesh
    void loadMesh(std::string fileName);
    void setMesh(Mesh *mesh, int eID);
    bool readFile(std::string fileName);
    bool readTriangleFile(std::string filename);
    Mesh *loadTriangleMesh(std::string fileName);

    // OctahedronBall functions
    void makeUnitOctahedron(GLint recursions);
    void subDivide(const gsl::Vector3D &a, const gsl::Vector3D &b, const gsl::Vector3D &c, GLint n);
    void makeTriangle(const gsl::Vector3D &v1, const gsl::Vector3D &v2, const gsl::Vector3D &v3);
    void makeLightMesh(int eID);
    void makeSkyBoxMesh(GLuint eID);
    void makePlaneMesh(GLuint eID);
    void makeBallMesh(GLuint eID, int n = 3);
};

#endif // RESOURCEMANAGER_H
