#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "components.h"
#include "shader.h"
#include "texture.h"
#include "tiny_obj_loader.h"
#include <QOpenGLFunctions_4_1_Core>
#include <memory>
#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif
class LightSystem;
class Entity;
class Registry;
class MainWindow;
class Scene;
typedef gsl::Vector3D vec3;
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

    std::map<ShaderType, Shader *> getShaders() const;

    void setLightSystem(LightSystem *lightSystem);

    std::vector<Component *> getComponents(int eID);

    QString getTextureName(GLuint id);

    QString getMeshName(const Mesh &mesh);

    void setLoading(bool load) { mLoading = load; }

    bool isLoading() const;

    // Basic Shapes and Prefabs
    GLuint makeXYZ(const QString &name = "XYZ");
    GLuint makeSkyBox(const QString &name = "Skybox");
    GLuint makeBillBoard(const QString &name = "BillBoard");
    GLuint makeOctBall(int n = 3);
    GLuint makePlane(const QString &name = "Plane");
    GLuint makeCube(const QString &name = "Cube");
    GLuint makeLightObject(const QString &name = "Light");
    GLuint make3DObject(std::string name, ShaderType type = Phong);
    GLuint makeTriangleSurface(std::string fileName, ShaderType type);

    Scene *getSceneLoader() const;

    void saveProjectSettings(const QString &fileName);

    QString getCurrentScene() const;

    QString getProjectName() const;

    void setCurrentScene(const QString &currentScene);

    void loadProject(const QString &fileName);

    void onExit();
    void loadLastProject();

private:
    // Private constructor
    ResourceManager();

    static ResourceManager *mInstance;
    Registry *registry;

    std::unique_ptr<Scene> mSceneLoader;
    QString mCurrentProject;
    QString mCurrentScene;
    QString mDefaultScene;

    bool mLoading{false};
    // std::map(key, object) for easy resource storage
    std::map<ShaderType, Shader *> mShaders;
    std::map<std::string, Texture *> mTextures;
    std::map<std::string, Mesh> mMeshMap; // Holds each unique mesh for easy access

    // Temp mVertices/mIndices container. Cleared before each use.
    meshData mMeshData;

    MainWindow *mMainWindow;
    //    std::vector<int> mGameObjectIndices;    // Holds the sparse array for gameobjects.

    // Systems
    LightSystem *mLightSystem;

    // OpenGL init functions
    void initVertexBuffers(Mesh *mesh);
    void initIndexBuffers(Mesh *mesh);

    // Reads and loads mesh
    void loadMesh(std::string fileName, GLuint eID);
    void setMesh(std::string name, GLuint eID);
    bool readFile(std::string fileName, GLuint eID);
    bool readTriangleFile(std::string filename, GLuint eID);
    void loadTriangleMesh(std::string fileName, GLuint eID);

    // OctahedronBall functions
    void makeUnitOctahedron(GLint recursions);
    void subDivide(const vec3 &a, const vec3 &b, const vec3 &c, GLint n);
    void makeTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3);
    void makeLightMesh(int eID);
    void makeSkyBoxMesh(GLuint eID);
    void makeXYZMesh(GLuint eID);
    void makePlaneMesh(GLuint eID);
    void makeBallMesh(GLuint eID, int n = 3);
    void makeBillBoardMesh(int eID);
    friend class ComponentList;
};

#endif // RESOURCEMANAGER_H
