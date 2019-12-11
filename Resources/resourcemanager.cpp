#include "resourcemanager.h"
#include "aisystem.h"
#include "cameracontroller.h"
#include "colorshader.h"
#include "innpch.h"
#include "inputsystem.h"
#include "mainwindow.h"
#include "movementsystem.h"
#include "registry.h"
#include "rendersystem.h"
#include "scene.h"
#include "scriptsystem.h"
#include "soundsystem.h"
#include "textureshader.h"
#include "tiny_obj_loader.h"
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QJSEngine> //The script engine itself!
#include <QStatusBar>
#include <QTimer>
#include <QToolButton>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <skyboxshader.h>

ResourceManager *ResourceManager::mInstance = nullptr;

ResourceManager::ResourceManager() : registry{Registry::instance()}
{

    registry->registerComponent<EInfo>();
    registry->registerComponent<Transform>();
    registry->registerComponent<Material>();
    registry->registerComponent<Mesh>();
    registry->registerComponent<ParticleEmitter>();
    registry->registerComponent<Physics>();
    registry->registerComponent<Sound>();
    registry->registerComponent<Input>();
    registry->registerComponent<Light>();
    registry->registerComponent<BSplinePoint>();
    registry->registerComponent<GameCamera>();
    registry->registerComponent<AIComponent>();
    registry->registerComponent<PlayerComponent>();
    registry->registerComponent<TowerComponent>();
    registry->registerComponent<BillBoard>();
    registry->registerComponent<Buildable>();
    registry->registerComponent<Bullet>();
    // Collision Types
    registry->registerComponent<AABB>();
    registry->registerComponent<Sphere>();

    mSceneLoader = std::make_unique<Scene>();
    createContext();        // create the sound context
    loadWave("gnomed.wav"); // initialize sounds you want to use here.
}
bool ResourceManager::createContext()
{
    qDebug() << "Intializing OpenAL!\n";
    mDevice = alcOpenDevice(NULL);
    if (mDevice) {
        mContext = alcCreateContext(mDevice, NULL);
        alcMakeContextCurrent(mContext);
    }

    // Generate buffers
    alGetError();

    if (!mDevice) {
        qDebug() << "Device not made!\n";
    }
    else
        qDebug() << "Intialization complete!\n";
    return true;
}
std::map<std::string, cjk::Ref<Texture>> ResourceManager::getTextures() const
{
    return mTextures;
}

cjk::Ref<CameraController> ResourceManager::getCurrentCameraController() const
{
    return mCurrentCameraController;
}

void ResourceManager::setCurrentCameraController(cjk::Ref<CameraController> currentCameraController)
{
    mCurrentCameraController = currentCameraController;
}

void ResourceManager::setCurrentScene(const QString &currentScene)
{
    mCurrentScene = currentScene;
}

QString ResourceManager::getProjectName() const
{
    return mCurrentProject;
}

QString ResourceManager::getCurrentScene() const
{
    return mCurrentScene;
}

Scene *ResourceManager::getSceneLoader() const
{
    return mSceneLoader.get();
}

bool ResourceManager::isLoading() const
{
    return mLoading;
}

// Do resource manager stuff -- Aka actually delete the pointers after application end
ResourceManager::~ResourceManager()
{
}
ResourceManager *ResourceManager::instance()
{
    if (!mInstance)
        mInstance = new ResourceManager();
    return mInstance;
}

void ResourceManager::saveProjectSettings(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    QFileInfo file{fileName};
    StringBuffer buf;
    PrettyWriter<StringBuffer> writer{buf};

    mCurrentProject = file.fileName();

    writer.StartObject(); // Start the json file
    writer.String(mCurrentProject.toStdString().c_str());
    writer.StartObject();
    writer.Key("default scene"); // Set the default startup scene.
    writer.String(mDefaultScene.toStdString().c_str());
    // add more options here as we think of them

    writer.EndObject();
    writer.EndObject(); // ready to write to rapidjson document

    std::ofstream of{gsl::settingsFilePath + file.fileName().toStdString() + ".json"};
    of << buf.GetString();
    if (!of.good() || !of)
        throw std::runtime_error("Can't write the JSON string to the file!");
}
void ResourceManager::loadProject(const QString &fileName)
{
    QFileInfo file{fileName};
    std::ifstream fileStream{gsl::settingsFilePath + file.fileName().toStdString()};
    if (!fileStream.good()) {
        qDebug() << "Can't read the JSON project file!";
        return;
    }
    std::stringstream stream;
    stream << fileStream.rdbuf();
    fileStream.close();
    const std::string fileCopy{stream.str()};
    const char *projSettings{fileCopy.c_str()};
    Document project;
    project.Parse(projSettings);
    mCurrentProject = QString::fromStdString(project.MemberBegin()->name.GetString());
    if (project[mCurrentProject.toStdString().c_str()].HasMember("default scene")) {
        mDefaultScene = project[mCurrentProject.toStdString().c_str()]["default scene"].GetString();
        mCurrentScene = mDefaultScene;
        mSceneLoader->loadScene(mDefaultScene + ".json");
    }
}
void ResourceManager::loadLastProject()
{
    std::ifstream fileStream{gsl::settingsFilePath + "EngineSettings/settings.json"};
    if (!fileStream.good()) {
        qDebug() << "Can't read the JSON settings file!";
        return;
    }
    std::stringstream stream;
    stream << fileStream.rdbuf();
    fileStream.close();
    const std::string fileCopy{stream.str()};
    const char *settings{fileCopy.c_str()};
    Document project;
    project.Parse(settings);
    mCurrentProject = project.MemberBegin()->value.GetString();
    loadProject(QString::fromStdString(gsl::settingsFilePath) + mCurrentProject + ".json");
}
void ResourceManager::onExit()
{
    StringBuffer buf;
    PrettyWriter<StringBuffer> writer{buf};

    writer.StartObject(); // Start the json file
    writer.Key("last project");
    writer.String(mCurrentProject.toStdString().c_str());
    writer.EndObject(); // ready to write to rapidjson document

    std::ofstream of{gsl::settingsFilePath + "EngineSettings/settings.json"};
    of << buf.GetString();
    if (!of.good() || !of)
        throw std::runtime_error("Can't write the JSON string to the file!");
}
/**
 * @brief Make a standard 3D object from a .obj or .txt file with the given name and type
 * @param name
 * @param type
 * @return The entity ID of the entity.
 */
GLuint ResourceManager::make3DObject(std::string name, cjk::Ref<Shader> type)
{
    if (name.find(".txt") != std::string::npos)
        return makeTriangleSurface(name, type);
    else {
        GLuint eID{registry->makeEntity<Transform>(QString::fromStdString(name))};
        registry->add<Material>(eID, type);
        addMeshComponent(name, eID);
        return eID;
    }
}
/**
 * @brief Plane prefab -- should fix coloring at some point
 * @return
 */
GLuint ResourceManager::makePlane(const QString &name)
{
    GLuint eID{registry->makeEntity<Transform>(name)};
    registry->add<Material>(eID, getShader<ColorShader>());
    auto search = mMeshMap.find("Plane");
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    }
    else
        makePlaneMesh(eID);

    return eID;
}

void ResourceManager::makePlaneMesh(GLuint eID)
{
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "Plane";
    mMeshData.mVertices.push_back(Vertex{1.0, 0, -1.0, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{-1.0, 0, -1.0, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{-1.0, 0, 1.0, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{-1.0, 0, 1.0, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{1.0, 0, 1.0, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{1.0, 0, -1.0, 0, 1, 0});

    // Once VAO and VBO have been generated, mMeshData can be discarded.
    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLES, mMeshData);
    auto &mesh{registry->get<Mesh>(eID)};

    // set up buffers (equivalent to init() from before)
    initVertexBuffers(&mesh);
    glBindVertexArray(0);
}
/**
 * @brief Cube prefab
 * @return
 */
GLuint ResourceManager::makeCube(const QString &name)
{
    GLuint eID{registry->makeEntity<Transform, Mesh>(name)};
    registry->add<Material>(eID, getShader<ColorShader>());
    registry->add<AABB>(eID, vec3{}, vec3{1.f, 1.f, 1.f});
    setMesh("cube.obj", eID);

    return eID;
}
/**
 * @brief Creates basic XYZ lines
 */
GLuint ResourceManager::makeXYZ(const QString &name)
{
    GLuint eID{registry->makeEntity<Transform, Mesh>(name)};
    registry->add<Material>(eID, getShader<ColorShader>());
    makeXYZMesh(eID);
    emit addedMesh(eID);

    return eID;
}
void ResourceManager::makeXYZMesh(GLuint eID)
{
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "XYZ";
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{100.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 100.f, 0.f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 0.f, 1.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 100.f, 0.f, 0.f, 1.f});

    // Once VAO and VBO have been generated, mMesh data can be discarded.
    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_LINES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_LINES, mMeshData);
    auto &mesh{registry->get<Mesh>(eID)};

    // set up buffers (equivalent to init() from before)
    initVertexBuffers(&mesh);
    glBindVertexArray(0);
}
/**
 * @brief Prefab skybox for editor
 * @return
 */
GLuint ResourceManager::makeSkyBox(const QString &name)
{
    GLuint eID{registry->makeEntity(name)};
    registry->add<Material>(eID, getShader<SkyboxShader>(), mTextures["Skybox"]->textureUnit());
    auto search = mMeshMap.find("Skybox");
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    }
    else
        makeSkyBoxMesh(eID);
    return eID;
}
void ResourceManager::makeSkyBoxMesh(GLuint eID)
{
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "Skybox";
    mMeshData.mVertices.insert(mMeshData.mVertices.end(),
                               {Vertex{vec3(-1.0f, 1.0f, -1.0f)},  // 0
                                Vertex{vec3(-1.0f, -1.0f, -1.0f)}, // 1
                                Vertex{vec3(1.0f, -1.0f, -1.0f)},  // 2
                                Vertex{vec3(1.0f, 1.0f, -1.0f)},   // 3
                                Vertex{vec3(-1.0f, -1.0f, 1.0f)},  // 4
                                Vertex{vec3(-1.0f, 1.0f, 1.0f)},   // 5
                                Vertex{vec3(1.0f, -1.0f, 1.0f)},   // 6
                                Vertex{vec3(1.0f, 1.0f, 1.0f)}});  // 7

    mMeshData.mIndices.insert(mMeshData.mIndices.end(),
                              {0, 1, 2, 2, 3, 0,
                               4, 1, 0, 0, 5, 4,
                               2, 6, 7, 7, 3, 2,
                               4, 5, 7, 7, 6, 4,
                               0, 3, 7, 7, 5, 0,
                               1, 4, 2, 2, 4, 6});

    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLES, mMeshData);

    auto &skyMesh{registry->get<Mesh>(eID)};

    initVertexBuffers(&skyMesh);
    initIndexBuffers(&skyMesh);

    glBindVertexArray(0);
}

/**
 * @brief Reads a .txt file for vertices.
 * @param fileName
 * @return Returns the entity id
 */
GLuint ResourceManager::makeTriangleSurface(std::string fileName, cjk::Ref<Shader> type)
{
    GLuint eID{registry->makeEntity<Transform, Mesh>(QString::fromStdString(fileName))};

    initializeOpenGLFunctions();

    registry->add<Material>(eID, type);
    setMesh(fileName, eID);
    glBindVertexArray(0);

    return eID;
}

GLuint ResourceManager::makeEnemy(const QString &name)
{
    GLuint eID{registry->makeEntity<Mesh, AIComponent>(name)};
    registry->add<Transform>(eID, vec3{}, vec3{}, vec3{0.3f, 0.3f, 0.3f});
    registry->add<AABB>(eID, vec3{0.f, 0.8f, -0.1f}, vec3{0.5f, 0.8f, 0.3f}, false);
    registry->add<Sound>(eID, "gnomed.wav", true);
    registry->get<Sound>(eID).playing = true;
    setMesh("OgreOBJ.obj", eID);
    registry->add<Material>(eID, getShader<TextureShader>(), mTextures["SkinColorMostro_COLOR.png"]->textureUnit()); // probably change textureunit later
    return eID;
}

/**
 * @brief Levelplane prefab
 * @param name
 * @return Returns the entity id
 */
GLuint ResourceManager::makeLevelPlane(const QString &name)
{
    GLuint eID{registry->makeEntity<Mesh, Buildable>(name)};
    registry->add<Transform>(eID, vec3{0}, vec3{}, vec3{2.5f, 1.f, 2.5f});
    registry->add<AABB>(eID, vec3{}, vec3{2.5f, 0.01f, 2.5f}, true);
    setMesh("Plane", eID);
    registry->add<Material>(eID, getShader<PhongShader>(), 0u, vec3{.57f, .57f, .57f});
    return eID;
}

/**
 * @brief Billboard prefab
 * @return
 */
GLuint ResourceManager::makeBillBoard(const QString &name)
{
    GLuint eID{registry->makeEntity<BillBoard>(name)};
    registry->add<Transform>(eID, vec3{4.f, 0.f, -3.5f});
    registry->add<Material>(eID, getShader<TextureShader>(), mTextures["gnome.bmp"]->textureUnit());
    auto search = mMeshMap.find("BillBoard");
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    }
    else
        makeBillBoardMesh(eID);

    return eID;
}
void ResourceManager::makeBillBoardMesh(int eID)
{
    initializeOpenGLFunctions();

    mMeshData.Clear();
    mMeshData.mName = "BillBoard";
    mMeshData.mVertices.insert(mMeshData.mVertices.end(), {
                                                              // Positions            // Normals          //UVs
                                                              Vertex{vec3(-2.f, -2.f, 0.f), vec3(0.0f, 0.0f, 1.0f), gsl::Vector2D(0.f, 0.f)}, // Bottom Left
                                                              Vertex{vec3(2.f, -2.f, 0.f), vec3(0.0f, 0.0f, 1.0f), gsl::Vector2D(1.f, 0.f)},  // Bottom Right
                                                              Vertex{vec3(-2.f, 2.f, 0.f), vec3(0.0f, 0.0f, 1.0f), gsl::Vector2D(0.f, 1.f)},  // Top Left
                                                              Vertex{vec3(2.f, 2.f, 0.f), vec3(0.0f, 0.0f, 1.0f), gsl::Vector2D(1.f, 1.f)}    // Top Right
                                                          });
    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLE_STRIP, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLE_STRIP, mMeshData);
    auto &billBoardMesh{registry->get<Mesh>(eID)};

    initVertexBuffers(&billBoardMesh);

    glBindVertexArray(0);
}

/**
 * @brief Creates the level grid.
 * @return
 */
void ResourceManager::makeLevel()
{
    float x{-16};
    float y{0};
    float z{-16};
    vec3 pos{x, y, z};
    int numPlanes{1};
    for (int i{0}; i < 16; i++) {

        for (int j{0}; j < 16; j++) {
            GLuint entityID = makeLevelPlane("Plane" + QString::number(numPlanes++));
            Transform &transform{registry->view<Transform>().get(entityID)};
            transform.localPosition = pos;
            transform.localPosition.x *= transform.localScale.x;
            transform.localPosition.z *= transform.localScale.z;
            pos.x = pos.x + 2;
        }
        pos.z = pos.z + 2;
        pos.x = -16;
    }
}

void ResourceManager::makeTowerMesh(GLuint eID)
{
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "Tower";
    mMeshData.mVertices.insert(mMeshData.mVertices.end(),
                               {
                                   //Vertex data for front
                                   Vertex{vec3{-0.8f, 0.f, -0.8f}, vec3{0.f, 0.f, 1.0f}, gsl::Vector2D{0.25f, 0.333f}}, //v0
                                   Vertex{vec3{0.8f, 0.8f, -0.8f}, vec3{0.f, 0.f, 1.0f}, gsl::Vector2D{0.5f, 0.333f}},  //v1
                                   Vertex{vec3{0.8f, 0.f, -0.8f}, vec3{0.f, 0.f, 1.0f}, gsl::Vector2D{0.25f, 0.666f}},  //v2
                                   Vertex{vec3{-0.8f, 0.8f, -0.8f}, vec3{0.f, 0.f, 1.0f}, gsl::Vector2D{0.5f, 0.666f}}, //v3

                                   //Vertex data for right
                                   Vertex{vec3{0.8f, 0.f, -0.8f}, vec3{1.0f, 0.f, 0.f}, gsl::Vector2D{0.5f, 0.333f}},   //v4
                                   Vertex{vec3{0.8f, 0.8f, 0.8f}, vec3{1.0f, 0.f, 0.f}, gsl::Vector2D{0.75f, 0.333f}},  //v5
                                   Vertex{vec3{0.8f, 0.f, 0.8f}, vec3{1.0f, 0.f, 0.f}, gsl::Vector2D{0.5f, 0.666f}},    //v6
                                   Vertex{vec3{0.8f, 0.8f, -0.8f}, vec3{1.0f, 0.f, 0.f}, gsl::Vector2D{0.75f, 0.666f}}, //v7

                                   //Vertex data for back
                                   Vertex{vec3{0.8f, 0.f, 0.8f}, vec3{0.f, 0.f, -1.0f}, gsl::Vector2D{0.75f, 0.333f}},  //v8
                                   Vertex{vec3{-0.8f, 0.8f, 0.8f}, vec3{0.f, 0.f, -1.0f}, gsl::Vector2D{0.8f, 0.333f}}, //v9
                                   Vertex{vec3{-0.8f, 0.f, 0.8f}, vec3{0.f, 0.f, -1.0f}, gsl::Vector2D{0.75f, 0.666f}}, //v10
                                   Vertex{vec3{0.8f, 0.8f, 0.8f}, vec3{0.f, 0.f, -1.0f}, gsl::Vector2D{0.8f, 0.666f}},  //v11

                                   //Vertex data for left
                                   Vertex{vec3{-0.8f, 0.f, 0.8f}, vec3{-1.0f, 0.f, 0.f}, gsl::Vector2D{0.f, 0.333f}},     //v12
                                   Vertex{vec3{-0.8f, 0.8f, -0.8f}, vec3{-1.0f, 0.f, 0.f}, gsl::Vector2D{0.25f, 0.333f}}, //v13
                                   Vertex{vec3{-0.8f, 0.f, -0.8f}, vec3{-1.0f, 0.f, 0.f}, gsl::Vector2D{0.f, 0.666f}},    //v14
                                   Vertex{vec3{-0.8f, 0.8f, 0.8f}, vec3{-1.0f, 0.f, 0.f}, gsl::Vector2D{0.25f, 0.666f}},  //v15

                                   //Vertex data for top
                                   Vertex{vec3{-0.8f, 0.8f, -0.8f}, vec3{0.f, 1.0f, 0.f}, gsl::Vector2D{0.25f, 0.666f}}, //v16
                                   Vertex{vec3{0.8f, 0.8f, 0.8f}, vec3{0.f, 1.0f, 0.f}, gsl::Vector2D{0.5f, 0.666f}},    //v17
                                   Vertex{vec3{0.8f, 0.8f, -0.8f}, vec3{0.f, 1.0f, 0.f}, gsl::Vector2D{0.25f, 0.999f}},  //v18
                                   Vertex{vec3{-0.8f, 0.8f, 0.8f}, vec3{0.f, 1.0f, 0.f}, gsl::Vector2D{0.5f, 0.999f}}    //v19
                               });

    mMeshData.mIndices.insert(mMeshData.mIndices.end(), {
                                                            0, 2, 1, 1, 2, 3,       //Face 0 - triangle strip (v0,  v1,  v2,  v3)
                                                            4, 6, 5, 5, 6, 7,       //Face 1 - triangle strip (v4,  v5,  v6,  v7)
                                                            8, 10, 9, 9, 10, 11,    //Face 2 - triangle strip (v8,  v9, v10,  v11)
                                                            12, 14, 13, 13, 14, 15, //Face 3 - triangle strip (v12, v13, v14, v15)
                                                            16, 18, 17, 17, 18, 19, //Face 4 - triangle strip (v16, v17, v18, v19)
                                                        });

    //    skyMat.setTextureUnit(Textures["skybox.bmp"]->id() - 1); // Not sure why the ID is one ahead of the actual texture I want??
    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLES, mMeshData);

    auto &towerMesh{registry->get<Mesh>(eID)};

    initVertexBuffers(&towerMesh);
    initIndexBuffers(&towerMesh);

    glBindVertexArray(0);
}

/**
 * @brief Sphere prefab
 * @param n - number of recursions. Increase number for "rounder" sphere
 * @return
 */
GLuint ResourceManager::makeOctBall(const QString &name, int n)
{
    GLuint eID{registry->makeEntity<Transform>(name)};

    registry->add<Material>(eID, getShader<ColorShader>());
    auto search{mMeshMap.find("Ball")};
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    }
    else
        makeBallMesh(eID, n);

    return eID;
}
void ResourceManager::makeBallMesh(GLuint eID, int n)
{
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "Ball";
    GLint mRecursions{n};

    makeUnitOctahedron(mRecursions); // This fills mMeshData

    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLES, mMeshData);

    auto &octMesh{registry->get<Mesh>(eID)};

    initVertexBuffers(&octMesh);
    initIndexBuffers(&octMesh);

    glBindVertexArray(0);
}
/**
 * @brief Light object prefab -- not fully implemented yet
 * @return
 */
GLuint ResourceManager::makeLightObject(const QString &name)
{
    GLuint eID{registry->makeEntity<Light>(name)};
    registry->add<Transform>(eID, vec3(2.5f, 3.f, 0.f), vec3(0.0f, 180.f, 0.0f));
    registry->add<Material>(eID, getShader<TextureShader>(), mTextures["white.bmp"]->textureUnit(), vec3(0.1f, 0.1f, 0.8f));
    auto search{mMeshMap.find("Pyramid")};
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    }
    else
        makeLightMesh(eID);
    return eID;
}
void ResourceManager::makeLightMesh(int eID)
{
    initializeOpenGLFunctions();

    mMeshData.Clear();
    mMeshData.mName = "Pyramid";

    mMeshData.mVertices.insert(mMeshData.mVertices.end(),
                               {
                                   //Vertex data - normals not correct
                                   Vertex{vec3{-0.5f, -0.5f, 0.5f}, vec3{0.f, 0.f, 1.0f}, gsl::Vector2D{0.f, 0.f}},  //Left low
                                   Vertex{vec3{0.5f, -0.5f, 0.5f}, vec3{0.f, 0.f, 1.0f}, gsl::Vector2D{1.f, 0.f}},   //Right low
                                   Vertex{vec3{0.0f, 0.5f, 0.0f}, vec3{0.f, 0.f, 1.0f}, gsl::Vector2D{0.5f, 0.5f}},  //Top
                                   Vertex{vec3{0.0f, -0.5f, -0.5f}, vec3{0.f, 0.f, 1.0f}, gsl::Vector2D{0.5f, 0.5f}} //Back low
                               });

    mMeshData.mIndices.insert(mMeshData.mIndices.end(),
                              {0, 1, 2,
                               1, 3, 2,
                               3, 0, 2,
                               0, 3, 1});
    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLES, mMeshData);
    auto &lightMesh{registry->get<Mesh>(eID)};

    initVertexBuffers(&lightMesh);
    initIndexBuffers(&lightMesh);

    glBindVertexArray(0);
}
void ResourceManager::setAABBMesh(Mesh &mesh)
{
    mMeshData.Clear();

    mMeshData.mVertices.insert(mMeshData.mVertices.end(),
                               {
                                   // Right face
                                   Vertex{vec3{1.0f, 1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.f, 0.f}},   //Left low
                                   Vertex{vec3{1.0f, -1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{1.f, 0.f}},  //Right low
                                   Vertex{vec3{1.0f, -1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}}, //Top
                                   Vertex{vec3{1.0f, 1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},  //Back low
                                   // 0,
                                   // Front face
                                   Vertex{vec3{1.0f, 1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},   //Top
                                   Vertex{vec3{1.0f, -1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},  //Back low
                                   Vertex{vec3{-1.0f, -1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}}, //Top
                                   Vertex{vec3{-1.0f, 1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},  //Back low
                                   // Left face
                                   Vertex{vec3{-1.0f, 1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},   //Top
                                   Vertex{vec3{-1.0f, -1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},  //Back low
                                   Vertex{vec3{-1.0f, -1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}}, //Top
                                   Vertex{vec3{-1.0f, 1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},  //Back low
                                   // Back face
                                   Vertex{vec3{-1.0f, 1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},  //Top
                                   Vertex{vec3{-1.0f, -1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}}, //Back low
                                   Vertex{vec3{1.0f, -1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},  //Top
                                   Vertex{vec3{1.0f, 1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},   //Back low
                                   // Top face
                                   Vertex{vec3{-1.0f, 1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}}, //Top
                                   Vertex{vec3{1.0f, 1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},  //Back low
                                   Vertex{vec3{1.0f, 1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},   //Top
                                   Vertex{vec3{-1.0f, 1.0f, 1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}},  //Back low
                                   Vertex{vec3{-1.0f, 1.0f, -1.0f}, vec3{0.f, 1.f, 0.f}, gsl::Vector2D{0.5f, 0.5f}}  //Back low
                               });
    mesh.mName = "BoxCollider";
    mesh.mVerticeCount = mMeshData.mVertices.size();
    mesh.mIndiceCount = mMeshData.mIndices.size();
    mesh.mDrawType = GL_LINE_STRIP;

    initVertexBuffers(&mesh);
    initIndexBuffers(&mesh);
}

/**
 * @brief opengl init - initialize the given mesh's buffers and arrays
 */
void ResourceManager::initVertexBuffers(Mesh *mesh)
{
    //Vertex Array Object - VAO
    glGenVertexArrays(1, &mesh->mVAO);
    glBindVertexArray(mesh->mVAO);

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers(1, &mesh->mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->mVBO);

    glBufferData(GL_ARRAY_BUFFER, mMeshData.mVertices.size() * sizeof(Vertex), mMeshData.mVertices.data(), GL_STATIC_DRAW);

    // 1rst attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
}
/**
 * @brief opengl init for glDrawElements - initialize the given mesh's index buffer
 * @param mesh
 */
void ResourceManager::initIndexBuffers(Mesh *mesh)
{
    glGenBuffers(1, &mesh->mEAB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mEAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mMeshData.mIndices.size() * sizeof(GLuint), mMeshData.mIndices.data(), GL_STATIC_DRAW);
}

void ResourceManager::initParticleBuffers(ParticleEmitter &emitter)
{
    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static std::vector<vec3> vertexBufferData = {{-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}};
    static std::vector<int> indices = {0, 1, 2, 3};
    glGenVertexArrays(1, &emitter.VAO);
    glBindVertexArray(emitter.VAO);
    glGenBuffers(1, &emitter.quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, emitter.quadVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferData.size() * sizeof(vec3), vertexBufferData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &emitter.EAB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, emitter.EAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &emitter.positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, emitter.positionBuffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, emitter.numParticles * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &emitter.colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, emitter.colorBuffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, emitter.numParticles * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);

    glBindVertexArray(0);
}

/**
 * @brief If you know the mesh you want at construction i.e. for prefabs and similar
 * @param name - name of the file you want to read
 * @param eID - entityID
 */
void ResourceManager::addMeshComponent(std::string name, GLuint eID)
{
    registry->add<Mesh>(eID);
    setMesh(name, eID);
    emit addedMesh(eID);
}
void ResourceManager::setMesh(std::string name, GLuint eID)
{
    if (name.empty())
        return;
    auto search{mMeshMap.find(name)};
    if (search != mMeshMap.end()) {
        registry->get<Mesh>(eID) = search->second;
        return;
    }
    else if (name == "Skybox")
        makeSkyBoxMesh(eID);
    else if (name == "BillBoard") {
        makeBillBoardMesh(eID);
    }
    else if (name == "Pyramid") // Light just refers to the pyramid mesh, probably not needed in the end
        makeLightMesh(eID);
    else if (name == "Plane")
        makePlaneMesh(eID);
    else if (name == "Ball")
        makeBallMesh(eID);
    else {
        if (name.find(".txt") != std::string::npos)
            loadTriangleMesh(name, eID);
        else
            loadMesh(name, eID);
    }
    // the mesh at the back is the latest creation
    mMeshMap[name] = registry->get<Mesh>(eID);
}

/**
 * @brief ResourceManager::LoadMesh - Loads the mesh from file if it isn't already in the Meshes map.
 * @param fileName
 * @return
 */
void ResourceManager::loadMesh(std::string fileName, int eID)
{
    if (!readFile(fileName, eID)) { // Should run readFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return;
    }
}

void ResourceManager::loadTriangleMesh(std::string fileName, GLuint eID)
{
    if (!readTriangleFile(fileName, eID)) { // Should run readTriangleFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return;
    }
}

bool ResourceManager::loadWave(std::string name)
{
    auto search = mSoundBuffers.find(name);
    if (search != mSoundBuffers.end()) { // file already loaded
        qDebug() << "Sound file already loaded!";
        return true;
    }
    ALuint frequency{};
    ALenum format{};
    wave_t *waveData{new wave_t()};
    if (!WavFileHandler::loadWave(gsl::soundFilePath + name, waveData)) {
        qDebug() << "Error loading wave file!\n";
        return false; // error loading wave file data
    }

    frequency = waveData->sampleRate;

    switch (waveData->bitsPerSample) {
    case 8:
        switch (waveData->channels) {
        case 1:
            format = AL_FORMAT_MONO8;
            //            qDebug() << "Format: 8bit Mono\n";
            break;
        case 2:
            format = AL_FORMAT_STEREO8;
            //            qDebug() << "Format: 8bit Stereo\n";
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (waveData->channels) {
        case 1:
            format = AL_FORMAT_MONO16;
            //            qDebug() << "Format: 16bit Mono\n";
            break;
        case 2:
            format = AL_FORMAT_STEREO16;
            //            qDebug() << "Format: 16bit Stereo\n";
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    if (waveData->buffer == nullptr) {
        qDebug() << "NO WAVE DATA!\n";
    }

    std::ostringstream i2s;
    i2s << waveData->dataSize;

    alGetError();
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, waveData->buffer, waveData->dataSize, frequency);
    mSoundBuffers[name] = buffer;

    if (waveData->buffer)
        delete waveData->buffer;
    if (waveData)
        delete waveData;
    return true;
}

void ResourceManager::initParticleEmitter(ParticleEmitter &emitter)
{
    initParticleBuffers(emitter);
}

/**
 * @brief Load texture if it's not already in storage.
 * @param fileName
 * @param textureUnit
 */
bool ResourceManager::loadTexture(std::string fileName)
{
    if (mTextures.find(fileName) == mTextures.end()) {
        cjk::Ref<Texture> tex{std::make_shared<Texture>(fileName, mTextures.size())};
        if (tex->isValid) {
            mTextures[fileName] = tex;
            qDebug() << "ResourceManager: Added texture" << QString::fromStdString(fileName);
            return true;
        }
        return false;
    }
    return false;
}
/**
 * @brief ResourceManager::loadCubemap specialized texture loader for the skybox
 * @param faces List of filenames for the cubemap
 * @return success
 */
bool ResourceManager::loadCubemap(std::vector<std::string> faces)
{
    if (mTextures.find("Skybox") == mTextures.end()) {
        cjk::Ref<Texture> tex{std::make_shared<Texture>(faces, mTextures.size())};
        if (tex->isValid) {
            mTextures["Skybox"] = tex;
            qDebug() << "ResourceManager: Added skybox cubemap";
            return true;
        }
        return false;
    }
    return false;
}
cjk::Ref<Texture> ResourceManager::getTexture(std::string fileName)
{
    return mTextures[fileName];
}

QString ResourceManager::getTextureName(GLuint textureUnit)
{
    for (auto it{mTextures.begin()}; it != mTextures.end(); ++it) {
        if (it->second->textureUnit() == textureUnit) {
            return QString::fromStdString(it->first);
        }
    }
    return QString();
}
QString ResourceManager::getMeshName(const Mesh &mesh)
{
    for (auto it{mMeshMap.begin()}; it != mMeshMap.end(); ++it) {
        if (it->second == mesh) {
            return QString::fromStdString(it->first);
        }
    }
    return QString();
}

Mesh ResourceManager::getMesh(std::string meshName)
{
    return mMeshMap[meshName];
}
/**
 * @brief Read .obj file.
 * @param fileName
 * @return
 */
bool ResourceManager::readFile(std::string fileName, int eID)
{
    //Open File
    std::string fileWithPath{gsl::assetFilePath + "Meshes/" + fileName};
    std::string mtlPath{gsl::assetFilePath + "Meshes/"};
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool ret{tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileWithPath.c_str(), mtlPath.c_str())};
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    if (!ret)
        return false;
    mMeshData.Clear();
    mMeshData.mName = fileName;
    // Append `default` material
    materials.push_back(tinyobj::material_t{});
    std::unordered_map<Vertex, GLuint> uniqueVertices;
    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex{};
            vertex.set_xyz(attrib.vertices[3 * index.vertex_index],
                           attrib.vertices[3 * index.vertex_index + 1],
                           attrib.vertices[3 * index.vertex_index + 2]);
            if (attrib.normals.size() != 0)
                vertex.set_normal(attrib.normals[3 * index.normal_index],
                                  attrib.normals[3 * index.normal_index + 1],
                                  attrib.normals[3 * index.normal_index + 2]);
            if (attrib.texcoords.size() != 0)
                vertex.set_st(attrib.texcoords[2 * index.texcoord_index],
                              attrib.texcoords[2 * index.texcoord_index + 1]);
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<GLuint>(mMeshData.mVertices.size());
                mMeshData.mVertices.push_back(vertex);
            }
            mMeshData.mIndices.push_back(uniqueVertices[vertex]);
        }
    }
    for (size_t m = 0; m < materials.size(); m++) {
        tinyobj::material_t *mp{&materials[m]};

        if (mp->diffuse_texname.length() > 0) {
            std::string texname{mp->diffuse_texname};
            auto search{mTextures.find(texname)};
            // Only load the texture if it is not already loaded
            bool textureLoaded{false};
            if (search == mTextures.end()) {
                textureLoaded = loadTexture(texname);
            }
            if (eID != -1) {
                // Currently doesn't support multiple textures
                if (registry->contains<Material>(eID) && textureLoaded) {
                    auto &mat{registry->get<Material>(eID)};
                    mat.mTextureUnit = mTextures[texname]->textureUnit();
                    mat.mShader = getShader<TextureShader>();
                }
            }
        }
    }
    Mesh temp{GL_TRIANGLES, mMeshData};
    initializeOpenGLFunctions();
    initVertexBuffers(&temp);
    initIndexBuffers(&temp);
    if (eID == -1) {
        mMeshMap[fileName] = temp;
        return true;
    }

    registry->get<Mesh>(eID) = temp;
    qDebug() << "Obj file read: " << QString::fromStdString(fileName);
    return true;
}

bool ResourceManager::readTriangleFile(std::string fileName, GLuint eID)
{
    std::ifstream inn;
    std::string fileWithPath{gsl::assetFilePath + "Meshes/" + fileName};
    mMeshData.Clear();
    inn.open(fileWithPath);

    if (inn.is_open()) {
        int n;
        Vertex vertex;
        inn >> n;

        mMeshData.mVertices.reserve(n);
        for (int i = 0; i < n; i++) {
            inn >> vertex;
            mMeshData.mVertices.push_back(vertex);
        }
        inn.close();
        qDebug() << "TriangleSurface file read: " << QString::fromStdString(fileName);

        auto &mesh{registry->get<Mesh>(eID)};
        mesh.mVerticeCount = mMeshData.mVertices.size();
        mesh.mName = fileName;
        mesh.mDrawType = GL_TRIANGLES;
        initVertexBuffers(&mesh);
        return true;
    }
    else {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(fileName);
        return false;
    }
}

std::map<std::string, cjk::Ref<Shader>> ResourceManager::getShaders() const
{
    return mShaders;
}
void ResourceManager::showMessage(const QString &message)
{
    mMainWindow->statusBar()->showMessage(message, 1000);
    mMainWindow->setShowingMsg(true);
    QTimer::singleShot(1000, this, &ResourceManager::changeMsg);
}

bool ResourceManager::isPaused() const
{
    return mIsPaused;
}

bool ResourceManager::isPlaying() const
{
    return mIsPlaying;
}
void ResourceManager::changeMsg()
{
    mMainWindow->setShowingMsg(!mMainWindow->showingMsg());
}

ALCcontext *ResourceManager::context() const
{
    return mContext;
}

void ResourceManager::clearContext()
{
    for (auto soundBuffer : getSoundBuffers()) {
        alDeleteBuffers(1, &soundBuffer.second);
    }
    mContext = alcGetCurrentContext();
    mDevice = alcGetContextsDevice(mContext);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(mContext);
    alcCloseDevice(mDevice);
}

ALCdevice *ResourceManager::device() const
{
    return mDevice;
}

std::map<std::string, ALuint> ResourceManager::getSoundBuffers() const
{
    return mSoundBuffers;
}
void ResourceManager::newScene(const QString &text)
{
    setCurrentScene(text);
    stop(); // Stop the editor if it's in play
    mMainWindow->clearEditor();
    registry->clearScene();
    save();
    getSceneLoader()->loadScene(getCurrentScene() + ".json");
}
void ResourceManager::newScene()
{
    QString text{QInputDialog::getText(mMainWindow, tr("New Scene"), tr("Scene name:"),
                                       QLineEdit::Normal, QString(), nullptr, Qt::MSWindowsFixedSizeDialogHint)};
    if (text.isEmpty()) {
        return;
    }
    newScene(text);
}
void ResourceManager::save()
{
    getSceneLoader()->saveScene(getCurrentScene());
    showMessage("Saved Scene!");
}
void ResourceManager::saveAs()
{
    QFileInfo file{QFileDialog::getSaveFileName(mMainWindow, tr("Save Scene"),
                                                QString::fromStdString(gsl::sceneFilePath), tr("JSON files (*.json)"))};
    if (file.fileName().isEmpty())
        return;
    mCurrentScene = file.fileName().chopped(5);
    save();
    mMainWindow->setWindowTitle(getProjectName() + " - Current Scene: " + getCurrentScene());
}
void ResourceManager::load()
{
    QFileInfo file{QFileDialog::getOpenFileName(mMainWindow, tr("Load Scene"),
                                                QString::fromStdString(gsl::sceneFilePath), tr("JSON files (*.json)"))};
    if (file.fileName().isEmpty())
        return;
    stop(); // Stop the editor if it's in play
    mMainWindow->clearEditor();
    getSceneLoader()->loadScene(file.fileName());
    setCurrentScene(file.fileName());
    Registry::instance()->system<MovementSystem>()->init();
    mMainWindow->setWindowTitle(getProjectName() + " - Current Scene: " + getCurrentScene());
    showMessage("Loaded Scene!");
}
void ResourceManager::loadProject()
{
    QFileInfo file{QFileDialog::getOpenFileName(mMainWindow, tr("Load Project"),
                                                QString::fromStdString(gsl::settingsFilePath), tr("JSON files (*.json)"))};
    if (file.fileName().isEmpty())
        return;
    stop(); // Stop the editor if it's in play
    mMainWindow->clearEditor();
    loadProject(file.fileName());
    mMainWindow->setWindowTitle(getProjectName() + " - Current Scene: " + getCurrentScene());
    Registry::instance()->system<MovementSystem>()->init();
    showMessage("Loaded Project: " + getProjectName());
}
void ResourceManager::saveProject()
{
    saveProjectSettings(QString::fromStdString(gsl::settingsFilePath + getProjectName().toStdString()));
    showMessage("Saved Project!");
}

void ResourceManager::newProject()
{
    QString text{QInputDialog::getText(mMainWindow, tr("New Project"), tr("Project name:"),
                                       QLineEdit::Normal, QString(), nullptr, Qt::MSWindowsFixedSizeDialogHint)};
    if (text.isEmpty()) {
        return;
    }
    mCurrentProject = text;
    mDefaultScene = "Untitled";
    newScene(mDefaultScene);
    Registry::instance()->system<MovementSystem>()->init();
    mMainWindow->setWindowTitle(getProjectName() + " - Current Scene: " + getCurrentScene());
    saveProject();
}
void ResourceManager::play()
{
    if (!mIsPlaying) {
        if (mIsPaused) { // Only make snapshot if not resuming from pause
            mIsPaused = false;
        }
        else
            Registry::instance()->makeSnapshot();
        auto [sound, input, renderer, ai]{registry->system<SoundSystem, InputSystem, RenderSystem, AISystem>()};
        sound->playAll();
        ai->resetTimers();
        for (auto controller : input->gameCameraControllers()) {
            if (controller->isActive()) {
                setActiveCameraController(controller);
                break;
            }
        }
        renderer->toggleRendered(0); // disables the XYZ indicators
        mIsPlaying = true;

        emit disableActions(true);
        emit disablePlay(true);
        emit disablePause(false);
        emit disableStop(false);
    }
}
void ResourceManager::pause()
{
    if (mIsPlaying) {
        mIsPlaying = false;
        mIsPaused = true;
        auto [sound, input]{registry->system<SoundSystem, InputSystem>()};
        input->setGameCameraInactive();
        setActiveCameraController(input->editorCamController());
        sound->pauseAll();

        emit disablePlay(false);
        emit disablePause(true);
    }
}
void ResourceManager::stop()
{
    if (mIsPlaying || mIsPaused) {
        auto [movement, sound, input]{registry->system<MovementSystem, SoundSystem, InputSystem>()};
        sound->stopAll();
        sound->refreshSounds();
        registry->loadSnapshot();

        movement->init();
        input->setGameCameraInactive();
        input->reset();
        setActiveCameraController(input->editorCamController());
        mIsPlaying = false;
        mMainWindow->insertEntities();

        emit disableActions(false);
        emit disablePlay(false);
        emit disablePause(true);
        emit disableStop(true);
    }
}
void ResourceManager::setActiveCameraController(cjk::Ref<CameraController> controller)
{
    for (auto shader : mShaders) {
        shader.second->setCameraController(controller);
    }
}
//=========================== Octahedron Functions =========================== //
void ResourceManager::makeTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3)
{
    mMeshData.mVertices.push_back(Vertex{v1, v1, gsl::Vector2D{0.f, 0.f}});
    mMeshData.mVertices.push_back(Vertex{v2, v2, gsl::Vector2D{1.f, 0.f}});
    mMeshData.mVertices.push_back(Vertex{v3, v3, gsl::Vector2D{0.5f, 1.f}});
}

void ResourceManager::subDivide(const vec3 &a, const vec3 &b, const vec3 &c, GLint n)
{
    if (n > 0) {
        vec3 v1{a + b};
        v1.normalize();

        vec3 v2{a + c};
        v2.normalize();

        vec3 v3{c + b};
        v3.normalize();
        subDivide(a, v1, v2, n - 1);
        subDivide(c, v2, v3, n - 1);
        subDivide(b, v3, v1, n - 1);
        subDivide(v3, v2, v1, n - 1);
    }
    else {
        makeTriangle(a, b, c);
    }
}

void ResourceManager::makeUnitOctahedron(GLint recursions)
{
    vec3 v0{0.f, 0.f, 1.f};
    vec3 v1{1.f, 0.f, 0.f};
    vec3 v2{0.f, 1.f, 0.f};
    vec3 v3{-1.f, 0.f, 0.f};
    vec3 v4{0.f, -1.f, 0.f};
    vec3 v5{0.f, 0.f, -1.f};

    subDivide(v0, v1, v2, recursions);
    subDivide(v0, v2, v3, recursions);
    subDivide(v0, v3, v4, recursions);
    subDivide(v0, v4, v1, recursions);
    subDivide(v5, v2, v1, recursions);
    subDivide(v5, v3, v2, recursions);
    subDivide(v5, v4, v3, recursions);
    subDivide(v5, v1, v4, recursions);
}
