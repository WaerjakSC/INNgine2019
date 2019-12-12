#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "components.h"
#include "core.h"
#include "phongshader.h"
#include "shader.h"
#include "texture.h"
#include "tiny_obj_loader.h"
#include <QOpenGLFunctions_4_1_Core>
#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif
class MainWindow;

class LightSystem;
class Registry;
class Scene;
class Script;
class CameraController;
class ResourceManager : public QObject, QOpenGLFunctions_4_1_Core {
    Q_OBJECT
    using vec3 = gsl::Vector3D;

public:
    static ResourceManager *instance();
    virtual ~ResourceManager();

    // Loads and generates shader (takes name and the shader)
    /**
     * @brief Load shader for the first time if it's not already in storage.
     * @param type
     * @param geometryPath
     */
    template <typename ShaderType>
    void loadShader(cjk::Ref<CameraController> camController, const GLchar *geometryPath = nullptr)
    {
        std::string shaderName{typeid(ShaderType).name()};
        if (mShaders.find(shaderName) == mShaders.end()) {
            mShaders[shaderName] = std::make_shared<ShaderType>(camController, geometryPath);
            qDebug() << "ResourceManager: Added shader " << QString::fromStdString(mShaders[shaderName]->getName());
        }
        else {
            qDebug() << "ResourceManager: Shader already loaded, ignoring...";
        }
    }
    // Gets stored shader
    template <typename Type>
    inline cjk::Ref<Type> getShader()
    {
        std::string type{typeid(Type).name()};
        return std::static_pointer_cast<Type>(mShaders[type]);
    }
    // Loads and generates texture from file
    bool loadTexture(std::string name);
    bool loadCubemap(std::vector<std::string> faces);

    // Gets stored texture
    cjk::Ref<Texture> getTexture(std::string name);

    QString getTextureName(GLuint textureUnit);

    void setMainWindow(MainWindow *window) { mMainWindow = window; }

    void addMeshComponent(std::string name, GLuint eID = -1);

    std::map<std::string, cjk::Ref<Shader>> getShaders() const;

    QString getMeshName(const Mesh &mesh);
    Mesh getMesh(std::string meshName);

    void setLoading(bool load) { mLoading = load; }

    bool isLoading() const;

    // Basic Shapes and Prefabs
    GLuint makeXYZ(const QString &name = "XYZ");
    GLuint makeSkyBox(const QString &name = "Skybox");
    GLuint makeBillBoard(const QString &name = "BillBoard");
    GLuint makeOctBall(const QString &name = "Ball", int n = 3);
    GLuint makePlane(const QString &name = "Plane");
    GLuint makeCube(const QString &name = "Cube");
    GLuint makeLightObject(const QString &name = "Light");
    GLuint make3DObject(std::string name, cjk::Ref<Shader> type = std::make_shared<PhongShader>());
    GLuint makeTriangleSurface(std::string fileName, cjk::Ref<Shader> type);
    GLuint makeEnemy(const QString &name = "Gnome");
    GLuint makeTower(const QString &name = "Tower");
    GLuint makeLevelPlane(const QString &name = "Level");
    GLuint makeTextureQuad(const QString &name = "TextureQuad");

    Scene *getSceneLoader() const;

    void saveProjectSettings(const QString &fileName);

    QString getCurrentScene() const;

    QString getProjectName() const;

    void setCurrentScene(const QString &currentScene);

    void loadProject(const QString &fileName);

    void onExit();
    void loadLastProject();
    bool isPlaying() const;

    bool isPaused() const;

    void loadMesh(std::string fileName, int eID = -1);

    void setCurrentCameraController(cjk::Ref<CameraController> currentCameraController);

    cjk::Ref<CameraController> getCurrentCameraController() const;
    void setActiveCameraController(cjk::Ref<CameraController> controller);

    /// Loads one given WAVE file.
    /**
        Calls the wave loader from the FileHandler class, parses the wave data and buffers it.
        \param The file path relative to execution directory.
    **/
    bool loadWave(std::string name);

    void initParticleEmitter(ParticleEmitter &emitter);
    void setAABBMesh(Mesh &mesh);
    void newScene(const QString &text);
    std::map<std::string, cjk::Ref<Texture>> getTextures() const;

    std::map<std::string, ALuint> getSoundBuffers() const;

    ALCdevice *device() const;

    ALCcontext *context() const;
    void clearContext();

public slots:
    void save();
    void load();
    void play();
    void pause();
    void stop();
    void saveAs();
    void saveProject();
    void loadProject();
    void makeLevel();

    void newProject();

    void newScene();
signals:
    void disableActions(bool disable);
    void disablePlay(bool disable);
    void disablePause(bool disable);
    void disableStop(bool disable);
    void addedMesh(GLuint entityID);

private:
    // Private constructor
    ResourceManager();

    static ResourceManager *mInstance;
    Registry *registry;

    cjk::Scope<Scene> mSceneLoader;
    QString mCurrentProject;
    QString mCurrentScene;
    QString mDefaultScene;
    cjk::Ref<CameraController> mCurrentCameraController;
    void changeMsg();

    bool mLoading{false};
    // std::map(key, object) for easy resource storage
    std::map<std::string, cjk::Ref<Shader>> mShaders;
    std::map<std::string, cjk::Ref<Texture>> mTextures;
    std::map<std::string, Mesh> mMeshMap; // Holds each unique mesh for easy access
    std::map<std::string, ALuint> mSoundBuffers;
    ALCdevice *mDevice{nullptr};   ///< Pointer to the ALC Device.
    ALCcontext *mContext{nullptr}; ///< Pointer to the ALC Context.
    bool createContext();

    // Temp mVertices/mIndices container. Cleared before each use.
    meshData mMeshData;

    MainWindow *mMainWindow;

    // Systems
    cjk::Ref<LightSystem> mLightSystem;
    void showMessage(const QString &message);
    bool mIsPlaying{false};
    bool mIsPaused{false}; // Don't make a snapshot if it was just restarted from a pause

    // OpenGL init functions
    void initVertexBuffers(Mesh *mesh);
    void initIndexBuffers(Mesh *mesh);

    void initParticleBuffers(ParticleEmitter &particle);
    // Reads and loads mesh
    void setMesh(std::string name, GLuint eID);
    bool readFile(std::string fileName, int eID = -1);
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
    void makeTextureQuadMesh(int eID);
    // void makeLevel(GLuint eID);
    void makeTowerMesh(GLuint eID);
    friend class ComponentList;
    friend class Scene;
};

#endif // RESOURCEMANAGER_H
