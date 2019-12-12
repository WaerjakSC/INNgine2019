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
    /**
     * Get a stored shader.
     */
    template <typename Type>
    inline cjk::Ref<Type> getShader()
    {
        std::string type{typeid(Type).name()};
        return std::static_pointer_cast<Type>(mShaders[type]);
    }
    /**
    * Load texture if it's not already in storage.
    * @param fileName
    * @param textureUnit
    */
    bool loadTexture(std::string name);
    /**
    * Specialized texture loader for the skybox.
    * @param faces List of filenames for the cubemap
    * @return success
    */
    bool loadCubemap(std::vector<std::string> faces);

    /**
     * Get a stored texture.
     * @param name
     * @return
     */
    cjk::Ref<Texture> getTexture(std::string name);
    /**
     * Get a texture's name by its texture unit.
     * @param textureUnit
     * @return
     */
    QString getTextureName(GLuint textureUnit);

    void setMainWindow(MainWindow *window) { mMainWindow = window; }
    /**
    * If you know the mesh you want at construction i.e. for prefabs and similar.
    * @param name - name of the file you want to read
    * @param eID - entityID
    */
    void addMeshComponent(std::string name, GLuint eID = -1);
    /**
     * Get an std::map containing all the shaders currently stored.
     * @return
     */
    std::map<std::string, cjk::Ref<Shader>> getShaders() const;
    /**
     * Get a mesh's name.
     * Usually refers to the name of the file it was loaded from.
     * @param mesh
     * @return
     */
    QString getMeshName(const Mesh &mesh);
    /**
     * Get a copy of a mesh from its name.
     * @param meshName
     * @return
     */
    Mesh getMesh(std::string meshName);

    void setLoading(bool load) { mLoading = load; }

    bool isLoading() const;

    // Basic Shapes and Prefabs
    /**
    * Creates basic XYZ lines.
    */
    GLuint makeXYZ(const QString &name = "XYZ");
    /**
     * Makes a skybox with the corresponding cubemap loaded in RenderWindow's init() function.
     * @param name
     * @return
     */
    GLuint makeSkyBox(const QString &name = "Skybox");
    /**
    * Billboard prefab.
    * @return
    */
    GLuint makeBillBoard(const QString &name = "BillBoard");
    /**
    * Sphere prefab.
    * @param n - number of recursions. Increase number for "rounder" sphere
    * @return
    */
    GLuint makeOctBall(const QString &name = "Ball", int n = 3);
    /**
    * Plane prefab.
    * @return
    */
    GLuint makePlane(const QString &name = "Plane");
    /**
    * Cube prefab.
    * @return
    */
    GLuint makeCube(const QString &name = "Cube");
    /**
    * Light object prefab -- not fully implemented yet.
    * @return
    */
    GLuint makeLightObject(const QString &name = "Light");
    /**
    * Make a standard 3D object from a .obj or .txt file with the given name and type.
    * @param name
    * @param type
    * @return The entity ID of the entity.
    */
    GLuint make3DObject(std::string name, cjk::Ref<Shader> type = std::make_shared<PhongShader>());

    /**
     * Reads a .txt file for vertices.
     * @param fileName
     * @param type
     * @return Returns the entity id
     */
    GLuint makeTriangleSurface(std::string fileName, cjk::Ref<Shader> type);
    /**
     * Enemy prefab.
     * Used to spawn enemies in the tower defense game.
     * @param name
     * @return
     */
    GLuint makeEnemy(const QString &name = "Gnome");
    /**
     * Tower prefab.
     * Includes a large Sphere collider as a range trigger.
     * @param name
     * @return
     */
    GLuint makeTower(const QString &name = "Tower");
    /**
    * Level Plane prefab.
    * @param name
    * @return Returns the entity id
    */
    GLuint makeLevelPlane(const QString &name = "Level");
    /**
    * TextureQuad prefab.
    * @param name
    * @return Returns the entity id
    */
    GLuint makeTextureQuad(const QString &name = "TextureQuad");

    /**
     * Returns a pointer to the scene loader.
     * @return
     */
    Scene *getSceneLoader() const;
    /**
     * Saves settings relating to the current project in a .json file.
     * @param fileName
     */
    void saveProjectSettings(const QString &fileName);

    QString getCurrentScene() const;

    QString getProjectName() const;

    void setCurrentScene(const QString &currentScene);

    /**
     * Loads a project and its corresponding default scene.
     * @param fileName
     */
    void loadProject(const QString &fileName);

    /**
     * Saves settings relating to the engine itself in a .json file.
     */
    void onExit();
    /**
     * Loads the project last used.
     */
    void loadLastProject();
    bool isPlaying() const;

    bool isPaused() const;
    /**
     * Loads the mesh from file if it isn't already in the Meshes map.
     * @param fileName
     * @return
    */
    void loadMesh(std::string fileName, int eID = -1);

    void setCurrentCameraController(cjk::Ref<CameraController> currentCameraController);

    cjk::Ref<CameraController> getCurrentCameraController() const;
    void setActiveCameraController(cjk::Ref<CameraController> controller);

    /**
        Calls the wave loader from the FileHandler class, parses the wave data and buffers it.
        \param The file path relative to execution directory.
    **/
    bool loadWave(std::string name);

    /**
     * Initializes a particle emitter with buffers and vertices.
     * @param emitter
     */
    void initParticleEmitter(ParticleEmitter &emitter);
    /**
     * Gives a mesh to an AABB collider.
     * @param mesh
     */
    void setAABBMesh(Mesh &mesh);
    /**
     * Creates a new empty scene.
     * @param text
     */
    void newScene(const QString &text);
    /**
     * Returns an std::map containing shared_ptr's to each loaded texture.
     * @return
     */
    std::map<std::string, cjk::Ref<Texture>> getTextures() const;

    /**
     * Get every loaded soundbuffer (each buffer should be a different sound.).
     * @return
     */
    std::map<std::string, ALuint> getSoundBuffers() const;

    ALCdevice *device() const;

    ALCcontext *context() const;
    /**
     * Clean up OpenAL stuff.
     */
    void clearContext();
    /**
     * Creates all the level planes.
     * Usually just a one-time use thing if you need to update all of them at the same time.
     */
    void makeLevel();

public slots:
    /**
     * Save a scene with the current entity and component settings.
     */
    void save();
    /**
     * Loads a previously saved scene from file.
     */
    void load();
    /**
     * Starts routines related to Play mode and stops certain others.
     * Also makes a snapshot of current entity and component settings.
     */
    void play();
    /**
     * Pause the Engine's play mode.
     * Returns the engine to edit mode.
     */
    void pause();
    /**
     * Returns the Engine to edit mode after loading the snapshot made by play().
     */
    void stop();
    /**
     * Saves the current scene as a new file with a user-chosen name.
     */
    void saveAs();
    /**
     * Saves the project with the current settings.
     */
    void saveProject();
    /**
     * Loads a saved project from file.
     */
    void loadProject();
    /**
     * Starts a new project with an empty scene.
     */
    void newProject();

    /**
     * Creates a new scene with a user-chosen name.
     */
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
    /**
     * Returns the message shown at the bottom of the renderwindow to showing the framerate.
     */
    void changeMsg();

    bool mLoading{false};
    // std::map(key, object) for easy resource storage
    std::map<std::string, cjk::Ref<Shader>> mShaders;
    std::map<std::string, cjk::Ref<Texture>> mTextures;
    std::map<std::string, Mesh> mMeshMap; /// Holds each unique mesh for easy access.
    std::map<std::string, ALuint> mSoundBuffers;
    ALCdevice *mDevice{nullptr};   ///< Pointer to the ALC Device.
    ALCcontext *mContext{nullptr}; ///< Pointer to the ALC Context.
    /**
     * Creates OpenAL context.
     * @return
     */
    bool createContext();

    // Temp mVertices/mIndices container. Cleared before each use.
    meshData mMeshData;

    MainWindow *mMainWindow;

    // Systems
    cjk::Ref<LightSystem> mLightSystem;
    /**
     * Change the message shown at the bottom of the renderwindow.
     * @param message
     */
    void showMessage(const QString &message);
    bool mIsPlaying{false};
    bool mIsPaused{false}; // Don't make a snapshot if it was just restarted from a pause

    /**
    * Initialize the given mesh's buffers and arrays.
    */
    void initVertexBuffers(Mesh *mesh);
    /**
    * Init for glDrawElements - initialize the given mesh's index buffer.
    * @param mesh
    */
    void initIndexBuffers(Mesh *mesh);

    void initParticleBuffers(ParticleEmitter &particle);
    /**
     * Reads and loads a mesh for an entity based on the name given.
     * @param name
     * @param eID
     */
    void setMesh(std::string name, GLuint eID);
    /**
    * Read .obj file.
    * @param fileName
    * @return
    */
    bool readFile(std::string fileName, int eID = -1);
    /**
     * Read a regular .txt file.
     * @param filename
     * @param eID
     * @return
     */
    bool readTriangleFile(std::string filename, GLuint eID);
    /**
     * Loads a regular .txt file, sends a qDebug message if loading failed.
     * @param fileName
     * @param eID
     */
    void loadTriangleMesh(std::string fileName, GLuint eID);
    /**
     * Make the actual mesh for the light object.
     * @param eID
     */
    void makeLightMesh(int eID);
    /**
     * Makes the actual skybox with correct vertex coordinates and indices.
     * @param eID
     */
    void makeSkyBoxMesh(GLuint eID);
    /**
     * Makes a simple line representation of the three main axes.
     * @param eID
     */
    void makeXYZMesh(GLuint eID);
    /**
     * Makes a simple plane mesh.
     * @param eID
     */
    void makePlaneMesh(GLuint eID);
    /**
     * Make an octahedron mesh.
     * @param eID
     * @param n Degree of roundness. Higher number means a rounder ball.
     */
    void makeBallMesh(GLuint eID, int n = 3);
    /**
     * Makes the mesh the billboard is shown on.
     * @param eID
     */
    void makeBillBoardMesh(int eID);
    /**
     * Makes a texture quad mesh.
     * @param eID
     */
    void makeTextureQuadMesh(int eID);
    // void makeLevel(GLuint eID);

    // OctahedronBall functions
    void makeUnitOctahedron(GLint recursions);
    void subDivide(const vec3 &a, const vec3 &b, const vec3 &c, GLint n);
    void makeTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3);

    friend class ComponentList;
    friend class Scene;
};

#endif // RESOURCEMANAGER_H
