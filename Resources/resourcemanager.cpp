#include "resourcemanager.h"
#include "billboard.h"
#include "cameracontroller.h"
#include "colorshader.h"
#include "innpch.h"
#include "inputsystem.h"
#include "lightsystem.h"
#include "mainwindow.h"
#include "movementsystem.h"
#include "rapidjson/prettywriter.h"
#include "registry.h"
#include "scene.h"
#include "scriptsystem.h"
#include "soundsystem.h"
#include "textureshader.h"
#include "tiny_obj_loader.h"
#include "wavfilehandler.h"
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QJSEngine> //The script engine itself!
#include <QStatusBar>
#include <QTimer>
#include <QToolButton>
#include <aisystem.h>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>

ResourceManager *ResourceManager::mInstance = nullptr;

ResourceManager::ResourceManager() {
    registry = Registry::instance();

    registry->registerComponent<Transform>();
    registry->registerComponent<Material>();
    registry->registerComponent<Mesh>();
    registry->registerComponent<Physics>();
    registry->registerComponent<Sound>();
    registry->registerComponent<Input>();
    registry->registerComponent<Light>();
    // Collision Types
    registry->registerComponent<AABB>();
    registry->registerComponent<OBB>();
    registry->registerComponent<Sphere>();
    registry->registerComponent<Plane>();
    registry->registerComponent<Cylinder>();
    registry->registerComponent<AIcomponent>();
    registry->registerComponent<BSplinePoint>();
    registry->registerComponent<GameCamera>();
    registry->registerComponent<PlayerComponent>();
    registry->registerComponent<TowerComponent>();
    
    mSceneLoader = std::make_unique<Scene>();

    // Beware of which class is created first - If ResourceManager is created first and starts making objects, it needs to register component types first.
    // On the other hand, if the systems are created first then you probably won't need to register anything in here, since those systems should take care of it.
}

Ref<CameraController> ResourceManager::getCurrentCameraController() const {
    return mCurrentCameraController;
}

void ResourceManager::setCurrentCameraController(Ref<CameraController> currentCameraController) {
    mCurrentCameraController = currentCameraController;
}

void ResourceManager::setCurrentScene(const QString &currentScene) {
    mCurrentScene = currentScene;
}

QString ResourceManager::getProjectName() const {
    return mCurrentProject;
}

QString ResourceManager::getCurrentScene() const {
    return mCurrentScene;
}

Scene *ResourceManager::getSceneLoader() const {
    return mSceneLoader.get();
}

bool ResourceManager::isLoading() const {
    return mLoading;
}

// Do resource manager stuff -- Aka actually delete the pointers after application end
ResourceManager::~ResourceManager() {
}
ResourceManager *ResourceManager::instance() {
    if (!mInstance)
        mInstance = new ResourceManager();
    return mInstance;
}

void ResourceManager::saveProjectSettings(const QString &fileName) {
    if (fileName.isEmpty())
        return;
    QFileInfo file(fileName);
    StringBuffer buf;
    PrettyWriter<StringBuffer> writer(buf);

    mCurrentProject = file.fileName();

    writer.StartObject(); // Start the json file
    writer.String(mCurrentProject.toStdString().c_str());
    writer.StartObject();
    writer.Key("default scene"); // Set the default startup scene.
    writer.String(mDefaultScene.toStdString().c_str());
    // add more options here as we think of them

    writer.EndObject();
    writer.EndObject(); // ready to write to rapidjson document

    std::ofstream of(gsl::settingsFilePath + file.fileName().toStdString() + ".json");
    of << buf.GetString();
    if (!of.good() || !of)
        throw std::runtime_error("Can't write the JSON string to the file!");
}
void ResourceManager::loadProject(const QString &fileName) {
    QFileInfo file(fileName);
    std::ifstream fileStream(gsl::settingsFilePath + file.fileName().toStdString());
    if (!fileStream.good()) {
        qDebug() << "Can't read the JSON project file!";
        return;
    }
    std::stringstream stream;
    stream << fileStream.rdbuf();
    fileStream.close();
    const std::string fileCopy = stream.str();
    const char *projSettings = fileCopy.c_str();
    Document project;
    project.Parse(projSettings);
    mCurrentProject = QString::fromStdString(project.MemberBegin()->name.GetString());
    if (project[mCurrentProject.toStdString().c_str()].HasMember("default scene")) {
        mDefaultScene = project[mCurrentProject.toStdString().c_str()]["default scene"].GetString();
        mCurrentScene = mDefaultScene;
        mSceneLoader->loadScene(mDefaultScene + ".json");
    }
}
void ResourceManager::loadLastProject() {
    std::ifstream fileStream(gsl::settingsFilePath + "EngineSettings/settings.json");
    if (!fileStream.good()) {
        qDebug() << "Can't read the JSON settings file!";
        return;
    }
    std::stringstream stream;
    stream << fileStream.rdbuf();
    fileStream.close();
    const std::string fileCopy = stream.str();
    const char *settings = fileCopy.c_str();
    Document project;
    project.Parse(settings);
    mCurrentProject = project.MemberBegin()->value.GetString();
    loadProject(QString::fromStdString(gsl::settingsFilePath) + mCurrentProject + ".json");
}
void ResourceManager::onExit() {
    StringBuffer buf;
    PrettyWriter<StringBuffer> writer(buf);

    writer.StartObject(); // Start the json file
    writer.Key("last project");
    writer.String(mCurrentProject.toStdString().c_str());
    writer.EndObject(); // ready to write to rapidjson document

    std::ofstream of(gsl::settingsFilePath + "EngineSettings/settings.json");
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
GLuint ResourceManager::make3DObject(std::string name, Ref<Shader> type) {
    if (name.find(".txt") != std::string::npos)
        return makeTriangleSurface(name, type);
    else {
        GLuint eID = registry->makeEntity(QString::fromStdString(name));
        registry->add<Transform>(eID);
        registry->add<Material>(eID, type);
        addMeshComponent(name, eID);
        return eID;
    }
}
/**
 * @brief Plane prefab -- should fix coloring at some point
 * @return
 */
GLuint ResourceManager::makePlane(const QString &name) {
    GLuint eID = registry->makeEntity(name);
    registry->add<Transform>(eID);
    registry->add<Material>(eID, getShader<ColorShader>());
    auto search = mMeshMap.find("Plane");
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    } else
        makePlaneMesh(eID);

    return eID;
}
void ResourceManager::makePlaneMesh(GLuint eID) {
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "Plane";
    mMeshData.mVertices.push_back(Vertex{0.8, 0, -0.8, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{-0.8, 0, -0.8, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{-0.8, 0, 0.8, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{-0.8, 0, 0.8, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{0.8, 0, 0.8, 0, 1, 0});
    mMeshData.mVertices.push_back(Vertex{0.8, 0, -0.8, 0, 1, 0});

    // Once VAO and VBO have been generated, mMeshData can be discarded.
    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLES, mMeshData);
    auto &mesh = registry->get<Mesh>(eID);

    // set up buffers (equivalent to init() from before)
    initVertexBuffers(&mesh);
    glBindVertexArray(0);
}
/**
 * @brief Cube prefab
 * @return
 */
GLuint ResourceManager::makeCube(const QString &name) {
    GLuint eID = registry->makeEntity(name);
    registry->add<Transform>(eID);
    registry->add<Material>(eID, getShader<ColorShader>());
    registry->add<Mesh>(eID);
    setMesh("Cube.obj", eID);

    return eID;
}
/**
 * @brief Creates basic XYZ lines
 */
GLuint ResourceManager::makeXYZ(const QString &name) {
    GLuint eID = registry->makeEntity(name);
    registry->add<Transform>(eID);
    registry->add<Material>(eID, getShader<ColorShader>());
    makeXYZMesh(eID);

    return eID;
}
void ResourceManager::makeXYZMesh(GLuint eID) {
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
    auto &mesh = registry->get<Mesh>(eID);

    // set up buffers (equivalent to init() from before)
    initVertexBuffers(&mesh);
    glBindVertexArray(0);
}
/**
 * @brief Prefab skybox for editor
 * @return
 */
GLuint ResourceManager::makeSkyBox(const QString &name) {
    GLuint eID = registry->makeEntity(name);
    registry->add<Transform>(eID, 0, 0, gsl::Vector3D(15));
    registry->add<Material>(eID, getShader<TextureShader>(), mTextures["skybox.bmp"]->id() - 1);
    auto search = mMeshMap.find("Skybox");
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    } else
        makeSkyBoxMesh(eID);
    return eID;
}
void ResourceManager::makeSkyBoxMesh(GLuint eID) {
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "Skybox";
    mMeshData.mVertices.insert(mMeshData.mVertices.end(),
    {
                                   //Vertex data for front
                                   Vertex{gsl::Vector3D(-1.f, -1.f, 1.f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.25f, 0.333f)}, //v0
                                   Vertex{gsl::Vector3D(1.f, -1.f, 1.f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.333f)},   //v1
                                   Vertex{gsl::Vector3D(-1.f, 1.f, 1.f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.25f, 0.666f)},  //v2
                                   Vertex{gsl::Vector3D(1.f, 1.f, 1.f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.666f)},    //v3

                                   //Vertex data for right
                                   Vertex{gsl::Vector3D(1.f, -1.f, 1.f), gsl::Vector3D(1.f, 0.f, 0.f), gsl::Vector2D(0.5f, 0.333f)},   //v4
                                   Vertex{gsl::Vector3D(1.f, -1.f, -1.f), gsl::Vector3D(1.f, 0.f, 0.f), gsl::Vector2D(0.75f, 0.333f)}, //v5
                                   Vertex{gsl::Vector3D(1.f, 1.f, 1.f), gsl::Vector3D(1.f, 0.f, 0.f), gsl::Vector2D(0.5f, 0.666f)},    //v6
                                   Vertex{gsl::Vector3D(1.f, 1.f, -1.f), gsl::Vector3D(1.f, 0.f, 0.f), gsl::Vector2D(0.75f, 0.666f)},  //v7

                                   //Vertex data for back
                                   Vertex{gsl::Vector3D(1.f, -1.f, -1.f), gsl::Vector3D(0.f, 0.f, -1.f), gsl::Vector2D(0.75f, 0.333f)}, //v8
                                   Vertex{gsl::Vector3D(-1.f, -1.f, -1.f), gsl::Vector3D(0.f, 0.f, -1.f), gsl::Vector2D(1.f, 0.333f)},  //v9
                                   Vertex{gsl::Vector3D(1.f, 1.f, -1.f), gsl::Vector3D(0.f, 0.f, -1.f), gsl::Vector2D(0.75f, 0.666f)},  //v10
                                   Vertex{gsl::Vector3D(-1.f, 1.f, -1.f), gsl::Vector3D(0.f, 0.f, -1.f), gsl::Vector2D(1.f, 0.666f)},   //v11

                                   //Vertex data for left
                                   Vertex{gsl::Vector3D(-1.f, -1.f, -1.f), gsl::Vector3D(-1.f, 0.f, 0.f), gsl::Vector2D(0.f, 0.333f)},  //v12
                                   Vertex{gsl::Vector3D(-1.f, -1.f, 1.f), gsl::Vector3D(-1.f, 0.f, 0.f), gsl::Vector2D(0.25f, 0.333f)}, //v13
                                   Vertex{gsl::Vector3D(-1.f, 1.f, -1.f), gsl::Vector3D(-1.f, 0.f, 0.f), gsl::Vector2D(0.f, 0.666f)},   //v14
                                   Vertex{gsl::Vector3D(-1.f, 1.f, 1.f), gsl::Vector3D(-1.f, 0.f, 0.f), gsl::Vector2D(0.25f, 0.666f)},  //v15

                                   //Vertex data for bottom
                                   Vertex{gsl::Vector3D(-1.f, -1.f, -1.f), gsl::Vector3D(0.f, -1.f, 0.f), gsl::Vector2D(0.25f, 0.f)},   //v16
                                   Vertex{gsl::Vector3D(1.f, -1.f, -1.f), gsl::Vector3D(0.f, -1.f, 0.f), gsl::Vector2D(0.5f, 0.f)},     //v17
                                   Vertex{gsl::Vector3D(-1.f, -1.f, 1.f), gsl::Vector3D(0.f, -1.f, 0.f), gsl::Vector2D(0.25f, 0.333f)}, //v18
                                   Vertex{gsl::Vector3D(1.f, -1.f, 1.f), gsl::Vector3D(0.f, -1.f, 0.f), gsl::Vector2D(0.5f, 0.333f)},   //v19

                                   //Vertex data for top
                                   Vertex{gsl::Vector3D(-1.f, 1.f, 1.f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.25f, 0.666f)},  //v20
                                   Vertex{gsl::Vector3D(1.f, 1.f, 1.f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.666f)},    //v21
                                   Vertex{gsl::Vector3D(-1.f, 1.f, -1.f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.25f, 0.999f)}, //v22
                                   Vertex{gsl::Vector3D(1.f, 1.f, -1.f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.999f)}    //v23
                               });

    mMeshData.mIndices.insert(mMeshData.mIndices.end(),
    {
                                  0, 2, 1, 1, 2, 3,       //Face 0 - triangle strip (v0,  v1,  v2,  v3)
                                  4, 6, 5, 5, 6, 7,       //Face 1 - triangle strip (v4,  v5,  v6,  v7)
                                  8, 10, 9, 9, 10, 11,    //Face 2 - triangle strip (v8,  v9, v10,  v11)
                                  12, 14, 13, 13, 14, 15, //Face 3 - triangle strip (v12, v13, v14, v15)
                                  16, 18, 17, 17, 18, 19, //Face 4 - triangle strip (v16, v17, v18, v19)
                                  20, 22, 21, 21, 22, 23  //Face 5 - triangle strip (v20, v21, v22, v23)
                              });

    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLES, mMeshData);

    auto &skyMesh = registry->get<Mesh>(eID);

    initVertexBuffers(&skyMesh);
    initIndexBuffers(&skyMesh);

    glBindVertexArray(0);
}

/**
 * @brief Reads a .txt file for vertices.
 * @param fileName
 * @return Returns the entity id
 */
GLuint ResourceManager::makeTriangleSurface(std::string fileName, Ref<Shader> type) {
    GLuint eID = registry->makeEntity(QString::fromStdString(fileName));

    initializeOpenGLFunctions();

    registry->add<Transform>(eID);
    registry->add<Material>(eID, type);
    registry->add<Mesh>(eID);
    setMesh(fileName, eID);
    glBindVertexArray(0);

    return eID;
}
/**
 * @brief Billboard prefab
 * @return
 */
GLuint ResourceManager::makeBillBoard(const QString &name) {
    GLuint eID = registry->makeEntity(name);
    registry->add<Transform>(eID, gsl::Vector3D(4.f, 0.f, -3.5f));
    registry->add<Material>(eID, getShader<TextureShader>(), mTextures["gnome.bmp"]->id() - 1);
    auto search = mMeshMap.find("BillBoard");
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    } else
        makeBillBoardMesh(eID);

    return eID;
}
void ResourceManager::makeBillBoardMesh(int eID) {
    initializeOpenGLFunctions();

    mMeshData.Clear();
    mMeshData.mName = "BillBoard";
    mMeshData.mVertices.insert(mMeshData.mVertices.end(), {
                                   // Positions            // Normals          //UVs
                                   Vertex{gsl::Vector3D(-2.f, -2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(0.f, 0.f)}, // Bottom Left
                                   Vertex{gsl::Vector3D(2.f, -2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(1.f, 0.f)},  // Bottom Right
                                   Vertex{gsl::Vector3D(-2.f, 2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(0.f, 1.f)},  // Top Left
                                   Vertex{gsl::Vector3D(2.f, 2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(1.f, 1.f)}    // Top Right
                               });
    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLE_STRIP, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLE_STRIP, mMeshData);
    auto &billBoardMesh = registry->get<Mesh>(eID);

    initVertexBuffers(&billBoardMesh);

    glBindVertexArray(0);
}

void ResourceManager::makeLevelMesh(GLuint eID) {
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "Level";
    std::vector<Vertex> vertices;
    int x = -8;
    int y = 0;
    int z = -8;
    for(int i = 0; i < 16; i++){

        for(int j = 0; j < 16; j++) {
            vertices.push_back(Vertex{gsl::Vector3D(x, y, z), gsl::Vector3D(0.f, 1.0f, 0.f), gsl::Vector2D(0.25f, 0.333f)});  //v0
            vertices.push_back(Vertex{gsl::Vector3D(x+1, y, z+1), gsl::Vector3D(0.f, 1.0f, 0.f), gsl::Vector2D(0.5f, 0.333f)});  //v1
            vertices.push_back(Vertex{gsl::Vector3D(x+1, y, z), gsl::Vector3D(0.f, 1.0f, 0.f), gsl::Vector2D(0.25f, 0.666f)}); //v2
            vertices.push_back(Vertex{gsl::Vector3D(x, y, z+1), gsl::Vector3D(0.f, 1.0f, 0.f), gsl::Vector2D(0.5f, 0.666f)}); // v4

            x = x+1;
        }
        z = z+1;
    }
    mMeshData.mVertices = vertices;
    std::vector<GLuint> indices;
    GLuint v = 0;
    for(int k = 0; k < 1024; k++) {
        indices.push_back(v);
        indices.push_back(v+2);
        indices.push_back(v+1);
        indices.push_back(v+1);
        indices.push_back(v+2);
        indices.push_back(v+3);

        v = v+4;
    }
    mMeshData.mIndices = indices;

    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLES, mMeshData);

    auto &levelMesh = registry->get<Mesh>(eID);

    initVertexBuffers(&levelMesh);
    initIndexBuffers(&levelMesh);

    glBindVertexArray(0);
}

void ResourceManager::makeTowerMesh(GLuint eID) {
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "Tower";
    mMeshData.mVertices.insert(mMeshData.mVertices.end(),
    {
                                   //Vertex data for front
                                   Vertex{gsl::Vector3D(-0.8f, 0.f, -0.8f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.25f, 0.333f)}, //v0
                                   Vertex{gsl::Vector3D(0.8f, 0.8f, -0.8f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.333f)},   //v1
                                   Vertex{gsl::Vector3D(0.8f, 0.f, -0.8f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.25f, 0.666f)},  //v2
                                   Vertex{gsl::Vector3D(-0.8f, 0.8f, -0.8f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.666f)},    //v3

                                   //Vertex data for right
                                   Vertex{gsl::Vector3D(0.8f, 0.f, -0.8f), gsl::Vector3D(1.0f, 0.f, 0.f), gsl::Vector2D(0.5f, 0.333f)},   //v4
                                   Vertex{gsl::Vector3D(0.8f, 0.8f, 0.8f), gsl::Vector3D(1.0f, 0.f, 0.f), gsl::Vector2D(0.75f, 0.333f)}, //v5
                                   Vertex{gsl::Vector3D(0.8f, 0.f, 0.8f), gsl::Vector3D(1.0f, 0.f, 0.f), gsl::Vector2D(0.5f, 0.666f)},    //v6
                                   Vertex{gsl::Vector3D(0.8f, 0.8f, -0.8f), gsl::Vector3D(1.0f, 0.f, 0.f), gsl::Vector2D(0.75f, 0.666f)},  //v7

                                   //Vertex data for back
                                   Vertex{gsl::Vector3D(0.8f, 0.f, 0.8f), gsl::Vector3D(0.f, 0.f, -1.0f), gsl::Vector2D(0.75f, 0.333f)}, //v8
                                   Vertex{gsl::Vector3D(-0.8f, 0.8f, 0.8f), gsl::Vector3D(0.f, 0.f, -1.0f), gsl::Vector2D(0.8f, 0.333f)},  //v9
                                   Vertex{gsl::Vector3D(-0.8f, 0.f, 0.8f), gsl::Vector3D(0.f, 0.f, -1.0f), gsl::Vector2D(0.75f, 0.666f)},  //v10
                                   Vertex{gsl::Vector3D(0.8f, 0.8f, 0.8f), gsl::Vector3D(0.f, 0.f, -1.0f), gsl::Vector2D(0.8f, 0.666f)},   //v11

                                   //Vertex data for left
                                   Vertex{gsl::Vector3D(-0.8f, 0.f, 0.8f), gsl::Vector3D(-1.0f, 0.f, 0.f), gsl::Vector2D(0.f, 0.333f)},  //v12
                                   Vertex{gsl::Vector3D(-0.8f, 0.8f, -0.8f), gsl::Vector3D(-1.0f, 0.f, 0.f), gsl::Vector2D(0.25f, 0.333f)}, //v13
                                   Vertex{gsl::Vector3D(-0.8f, 0.f, -0.8f), gsl::Vector3D(-1.0f, 0.f, 0.f), gsl::Vector2D(0.f, 0.666f)},   //v14
                                   Vertex{gsl::Vector3D(-0.8f, 0.8f, 0.8f), gsl::Vector3D(-1.0f, 0.f, 0.f), gsl::Vector2D(0.25f, 0.666f)},  //v15

                                   //Vertex data for top
                                   Vertex{gsl::Vector3D(-0.8f, 0.8f, -0.8f), gsl::Vector3D(0.f, 1.0f, 0.f), gsl::Vector2D(0.25f, 0.666f)},  //v16
                                   Vertex{gsl::Vector3D(0.8f, 0.8f, 0.8f), gsl::Vector3D(0.f, 1.0f, 0.f), gsl::Vector2D(0.5f, 0.666f)},    //v17
                                   Vertex{gsl::Vector3D(0.8f, 0.8f, -0.8f), gsl::Vector3D(0.f, 1.0f, 0.f), gsl::Vector2D(0.25f, 0.999f)}, //v18
                                   Vertex{gsl::Vector3D(-0.8f, 0.8f, 0.8f), gsl::Vector3D(0.f, 1.0f, 0.f), gsl::Vector2D(0.5f, 0.999f)}    //v19
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

    auto &towerMesh = registry->get<Mesh>(eID);

    initVertexBuffers(&towerMesh);
    initIndexBuffers(&towerMesh);

    glBindVertexArray(0);
}

/**
 * @brief Sphere prefab
 * @param n - number of recursions. Increase number for "rounder" sphere
 * @return
 */
GLuint ResourceManager::makeOctBall(const QString &name, int n) {
    GLuint eID = registry->makeEntity(name);

    registry->add<Transform>(eID);
    registry->add<Material>(eID, getShader<ColorShader>());
    auto search = mMeshMap.find("Ball");
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    } else
        makeBallMesh(eID, n);

    return eID;
}
void ResourceManager::makeBallMesh(GLuint eID, int n) {
    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mName = "Ball";
    GLint mRecursions = n;
    GLint mIndex = 0;

    GLuint mNumberVertices = static_cast<GLuint>(3 * 8 * std::pow(4, mRecursions)); // Not sure what these are used for?

    makeUnitOctahedron(mRecursions); // This fills mMeshData

    if (!registry->contains<Mesh>(eID))
        registry->add<Mesh>(eID, GL_TRIANGLES, mMeshData);
    else
        registry->get<Mesh>(eID) = Mesh(GL_TRIANGLES, mMeshData);

    auto &octMesh = registry->get<Mesh>(eID);

    initVertexBuffers(&octMesh);
    initIndexBuffers(&octMesh);

    glBindVertexArray(0);
}
/**
 * @brief Light object prefab -- not fully implemented yet
 * @return
 */
GLuint ResourceManager::makeLightObject(const QString &name) {
    GLuint eID = registry->makeEntity(name);
    registry->add<Transform>(eID, gsl::Vector3D(2.5f, 3.f, 0.f), gsl::Vector3D(0.0f, 180.f, 0.0f));
    registry->add<Material>(eID, getShader<TextureShader>(), mTextures["white.bmp"]->id() - 1, gsl::Vector3D(0.1f, 0.1f, 0.8f));
    registry->add<Light>(eID);
    auto search = mMeshMap.find("Pyramid");
    if (search != mMeshMap.end()) {
        registry->add<Mesh>(eID, search->second);
    } else
        makeLightMesh(eID);
    return eID;
}
void ResourceManager::makeLightMesh(int eID) {
    initializeOpenGLFunctions();

    mMeshData.Clear();
    mMeshData.mName = "Pyramid";

    mMeshData.mVertices.insert(mMeshData.mVertices.end(),
    {
                                   //Vertex data - normals not correct
                                   Vertex{gsl::Vector3D(-0.5f, -0.5f, 0.5f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.f, 0.f)},  //Left low
                                   Vertex{gsl::Vector3D(0.5f, -0.5f, 0.5f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(1.f, 0.f)},   //Right low
                                   Vertex{gsl::Vector3D(0.0f, 0.5f, 0.0f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.5f)},  //Top
                                   Vertex{gsl::Vector3D(0.0f, -0.5f, -0.5f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.5f)} //Back low
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
    auto &lightMesh = registry->get<Mesh>(eID);

    initVertexBuffers(&lightMesh);
    initIndexBuffers(&lightMesh);

    glBindVertexArray(0);
}
void ResourceManager::setColliderMesh(Mesh &mesh) {
    mMeshData.Clear();

    mMeshData.mVertices.insert(mMeshData.mVertices.end(),
    {
                                   // Right face
                                   Vertex{gsl::Vector3D(1.0f, 1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.f, 0.f)},   //Left low
                                   Vertex{gsl::Vector3D(1.0f, -1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(1.f, 0.f)},  //Right low
                                   Vertex{gsl::Vector3D(1.0f, -1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)}, //Top
                                   Vertex{gsl::Vector3D(1.0f, 1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},  //Back low
                                   // 0,
                                   // Front face
                                   Vertex{gsl::Vector3D(1.0f, 1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},   //Top
                                   Vertex{gsl::Vector3D(1.0f, -1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},  //Back low
                                   Vertex{gsl::Vector3D(-1.0f, -1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)}, //Top
                                   Vertex{gsl::Vector3D(-1.0f, 1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},  //Back low
                                   // Left face
                                   Vertex{gsl::Vector3D(-1.0f, 1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},   //Top
                                   Vertex{gsl::Vector3D(-1.0f, -1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},  //Back low
                                   Vertex{gsl::Vector3D(-1.0f, -1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)}, //Top
                                   Vertex{gsl::Vector3D(-1.0f, 1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},  //Back low
                                   // Back face
                                   Vertex{gsl::Vector3D(-1.0f, 1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},  //Top
                                   Vertex{gsl::Vector3D(-1.0f, -1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)}, //Back low
                                   Vertex{gsl::Vector3D(1.0f, -1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},  //Top
                                   Vertex{gsl::Vector3D(1.0f, 1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},   //Back low
                                   // Top face
                                   Vertex{gsl::Vector3D(-1.0f, 1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},  //Top
                                   Vertex{gsl::Vector3D(1.0f, 1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},   //Back low
                                   Vertex{gsl::Vector3D(1.0f, 1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},    //Top
                                   Vertex{gsl::Vector3D(-1.0f, 1.0f, 1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)},   //Back low
                                   Vertex{gsl::Vector3D(-1.0f, 1.0f, -1.0f), gsl::Vector3D(0.f, 1.f, 0.f), gsl::Vector2D(0.5f, 0.5f)}   //Back low
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
void ResourceManager::initVertexBuffers(Mesh *mesh) {
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
void ResourceManager::initIndexBuffers(Mesh *mesh) {
    glGenBuffers(1, &mesh->mEAB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mEAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mMeshData.mIndices.size() * sizeof(GLuint), mMeshData.mIndices.data(), GL_STATIC_DRAW);
}

/**
 * @brief If you know the mesh you want at construction i.e. for prefabs and similar
 * @param name - name of the file you want to read
 * @param eID - entityID
 */
void ResourceManager::addMeshComponent(std::string name, int eID) {
    if (eID <= -1 || (size_t)eID > registry->numEntities() - 1) {
        eID = registry->numEntities() - 1;
    }
    registry->add<Mesh>(eID);
    setMesh(name, eID);
}
void ResourceManager::setMesh(std::string name, GLuint eID) {
    if (name.empty())
        return;
    auto search = mMeshMap.find(name);
    if (search != mMeshMap.end()) {
        registry->get<Mesh>(eID) = search->second;
        return;
    } else if (name == "Skybox")
        makeSkyBoxMesh(eID);
    else if (name == "BillBoard") {
        makeBillBoardMesh(eID);
        registry->addBillBoard(eID);
    } else if (name == "Pyramid") // Light just refers to the pyramid mesh, probably not needed in the end
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
void ResourceManager::loadMesh(std::string fileName, GLuint eID) {
    if (!readFile(fileName, eID)) { // Should run readFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return;
    }
}

void ResourceManager::loadTriangleMesh(std::string fileName, GLuint eID) {
    if (!readTriangleFile(fileName, eID)) { // Should run readTriangleFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return;
    }
}

bool ResourceManager::loadWave(std::string filePath, Sound &sound) {
    ALuint frequency{};
    ALenum format{};
    wave_t *waveData = new wave_t();
    if (!WavFileHandler::loadWave(filePath, waveData)) {
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

    if (waveData->buffer == NULL) {
        qDebug() << "NO WAVE DATA!\n";
    }

    std::ostringstream i2s;
    i2s << waveData->dataSize;
    //    qDebug() << "DataSize: " << QString::fromStdString(i2s.str()) << " bytes\n";

    alGetError();
    alBufferData(sound.mBuffer, format, waveData->buffer, waveData->dataSize, frequency);
    SoundSystem *soundSys = registry->getSystem<SoundSystem>().get();
    soundSys->checkError("alBufferData");
    alSourcei(sound.mSource, AL_BUFFER, sound.mBuffer);
    soundSys->checkError("alSourcei (loadWave)");

    //    qDebug() << "Loading complete!\n";
    if (waveData->buffer)
        delete waveData->buffer;
    if (waveData)
        delete waveData;
    return true;
}

/**
 * @brief Load texture if it's not already in storage.
 * @param fileName
 * @param textureUnit
 */
bool ResourceManager::loadTexture(std::string fileName) {
    if (mTextures.find(fileName) == mTextures.end()) {
        Ref<Texture> tex = std::make_shared<Texture>(fileName, mTextures.size());
        if (tex->isValid) {
            mTextures[fileName] = tex;
            qDebug() << "ResourceManager: Added texture" << QString::fromStdString(fileName);
            return true;
        }
        return false;
    }
    return false;
}

Ref<Texture> ResourceManager::getTexture(std::string fileName) {
    return mTextures[fileName];
}
QString ResourceManager::getTextureName(GLuint id) {
    for (auto it = mTextures.begin(); it != mTextures.end(); ++it) {
        if (it->second->id() == id + 1) {
            return QString::fromStdString(it->first);
        }
    }
    return QString();
}
QString ResourceManager::getMeshName(const Mesh &mesh) {
    for (auto it = mMeshMap.begin(); it != mMeshMap.end(); ++it) {
        if (it->second == mesh) {
            return QString::fromStdString(it->first);
        }
    }
    return QString();
}
/**
 * @brief Read .obj file.
 * @param fileName
 * @return
 */
bool ResourceManager::readFile(std::string fileName, GLuint eID) {
    //Open File
    std::string fileWithPath = gsl::assetFilePath + "Meshes/" + fileName;
    std::string mtlPath = gsl::assetFilePath + "Meshes/";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileWithPath.c_str(), mtlPath.c_str());
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    if (!ret)
        return false;
    mMeshData.Clear();
    // Append `default` material
    materials.push_back(tinyobj::material_t());
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
        tinyobj::material_t *mp = &materials[m];

        if (mp->diffuse_texname.length() > 0) {
            auto search = mTextures.find(mp->diffuse_texname);
            // Only load the texture if it is not already loaded
            bool textureLoaded = false;
            if (search == mTextures.end()) {
                textureLoaded = loadTexture(mp->diffuse_texname);
            }
            // Currently doesn't support multiple textures
            if (registry->contains<Material>(eID) && textureLoaded) {
                auto &mat = registry->get<Material>(eID);
                mat.mTextureUnit = search->second->id() - 1;
                mat.mShader = getShader<TextureShader>();
            }
        }
    }

    auto &mesh = registry->get<Mesh>(eID);
    mesh.mName = fileName;
    mesh.mVerticeCount = mMeshData.mVertices.size();
    mesh.mIndiceCount = mMeshData.mIndices.size();
    mesh.mDrawType = GL_TRIANGLES;

    initVertexBuffers(&mesh);
    initIndexBuffers(&mesh);

    qDebug() << "Obj file read: " << QString::fromStdString(fileName);
    return true;
}

bool ResourceManager::readTriangleFile(std::string fileName, GLuint eID) {
    std::ifstream inn;
    std::string fileWithPath = gsl::assetFilePath + "Meshes/" + fileName;
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

        auto &mesh = registry->get<Mesh>(eID);
        mesh.mVerticeCount = mMeshData.mVertices.size();
        mesh.mName = fileName;
        mesh.mDrawType = GL_TRIANGLES;
        initVertexBuffers(&mesh);
        return true;
    } else {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(fileName);
        return false;
    }
}

std::map<std::string, Ref<Shader>> ResourceManager::getShaders() const {
    return mShaders;
}
void ResourceManager::showMessage(const QString &message) {
    mMainWindow->statusBar()->showMessage(message, 1000);
    mMainWindow->mShowingMsg = true;
    QTimer::singleShot(1000, this, &ResourceManager::changeMsg);
}

bool ResourceManager::getPaused() const {
    return mPaused;
}

bool ResourceManager::isPlaying() const {
    return mIsPlaying;
}
void ResourceManager::changeMsg() {
    bool &msg = mMainWindow->mShowingMsg;
    msg = !msg;
}
void ResourceManager::newScene(const QString &text) {
    setCurrentScene(text);
    stop(); // Stop the editor if it's in play
    mMainWindow->clearEditor();
    registry->clearScene();
    save();
    getSceneLoader()->loadScene(getCurrentScene() + ".json");
}
void ResourceManager::newScene() {
    QString text = QInputDialog::getText(mMainWindow, tr("New Scene"), tr("Scene name:"), QLineEdit::Normal, QString(), nullptr, Qt::MSWindowsFixedSizeDialogHint);
    if (text.isEmpty()) {
        return;
    }
    newScene(text);
}
void ResourceManager::save() {
    getSceneLoader()->saveScene(getCurrentScene());
    showMessage("Saved Scene!");
}
void ResourceManager::saveAs() {
    QFileInfo file(QFileDialog::getSaveFileName(mMainWindow, tr("Save Scene"), QString::fromStdString(gsl::sceneFilePath), tr("JSON files (*.json)")));
    if (file.fileName().isEmpty())
        return;
    mCurrentScene = file.fileName().chopped(5);
    save();
    mMainWindow->setWindowTitle(getProjectName() + " - Current Scene: " + getCurrentScene());
}
void ResourceManager::load() {
    QFileInfo file(QFileDialog::getOpenFileName(mMainWindow, tr("Load Scene"), QString::fromStdString(gsl::sceneFilePath), tr("JSON files (*.json)")));
    if (file.fileName().isEmpty())
        return;
    stop(); // Stop the editor if it's in play
    mMainWindow->clearEditor();
    getSceneLoader()->loadScene(file.fileName());
    setCurrentScene(file.fileName());
    Registry::instance()->getSystem<MovementSystem>()->init();
    mMainWindow->setWindowTitle(getProjectName() + " - Current Scene: " + getCurrentScene());
    showMessage("Loaded Scene!");
}
void ResourceManager::loadProject() {
    QFileInfo file(QFileDialog::getOpenFileName(mMainWindow, tr("Load Project"), QString::fromStdString(gsl::settingsFilePath), tr("JSON files (*.json)")));
    if (file.fileName().isEmpty())
        return;
    stop(); // Stop the editor if it's in play
    mMainWindow->clearEditor();
    loadProject(file.fileName());
    mMainWindow->setWindowTitle(getProjectName() + " - Current Scene: " + getCurrentScene());
    Registry::instance()->getSystem<MovementSystem>()->init();
    showMessage("Loaded Project: " + getProjectName());
}
void ResourceManager::saveProject() {
    saveProjectSettings(QString::fromStdString(gsl::settingsFilePath + getProjectName().toStdString()));
    showMessage("Saved Project!");
}

void ResourceManager::newProject() {
    QString text = QInputDialog::getText(mMainWindow, tr("New Project"), tr("Project name:"), QLineEdit::Normal, QString(), nullptr, Qt::MSWindowsFixedSizeDialogHint);
    if (text.isEmpty()) {
        return;
    }
    mCurrentProject = text;
    mDefaultScene = "Untitled";
    newScene(mDefaultScene);
    Registry::instance()->getSystem<MovementSystem>()->init();
    mMainWindow->setWindowTitle(getProjectName() + " - Current Scene: " + getCurrentScene());
    saveProject();
}
void ResourceManager::play() {
    if (!mIsPlaying) {
        if (mPaused) { // Only make snapshot if not resuming from pause
            mPaused = false;
        } else
            Registry::instance()->makeSnapshot();
        registry->getSystem<SoundSystem>()->playAll();
        auto inputsys = registry->getSystem<InputSystem>();
        for (auto shader : mShaders) {
            for (auto controller : inputsys->gameCameraControllers()) {
                if (controller->mActive) {
                    shader.second->setCameraController(controller);
                    break;
                }
            }
        }
        mIsPlaying = true;
        registry->getSystem<AIsystem>()->masterOfCurves();
        mMainWindow->play->setEnabled(false);
        mMainWindow->pause->setEnabled(true);
        mMainWindow->stop->setEnabled(true);
    }
}
void ResourceManager::pause() {
    if (mIsPlaying) {
        mIsPlaying = false;
        mPaused = true;
        mMainWindow->play->setEnabled(true);
        mMainWindow->pause->setEnabled(false);
        registry->getSystem<SoundSystem>()->pauseAll();
    }
}
void ResourceManager::stop() {
    if (mIsPlaying) {
        registry->loadSnapshot();
        registry->getSystem<MovementSystem>()->init();
        registry->getSystem<SoundSystem>()->stopAll();
        auto inputsys = registry->getSystem<InputSystem>();
        for (auto shader : mShaders) {
            shader.second->setCameraController(inputsys->editorCamController());
        }
        mIsPlaying = false;
        mMainWindow->insertEntities();
        mMainWindow->play->setEnabled(true);
        mMainWindow->pause->setEnabled(false);
        mMainWindow->stop->setEnabled(false);
    }
}

//=========================== Octahedron Functions =========================== //
void ResourceManager::makeTriangle(const gsl::Vector3D &v1, const gsl::Vector3D &v2, const gsl::Vector3D &v3) {
    mMeshData.mVertices.push_back(Vertex(v1, v1, gsl::Vector2D(0.f, 0.f)));
    mMeshData.mVertices.push_back(Vertex(v2, v2, gsl::Vector2D(1.f, 0.f)));
    mMeshData.mVertices.push_back(Vertex(v3, v3, gsl::Vector2D(0.5f, 1.f)));
}

void ResourceManager::subDivide(const gsl::Vector3D &a, const gsl::Vector3D &b, const gsl::Vector3D &c, GLint n) {
    if (n > 0) {
        gsl::Vector3D v1 = a + b;
        v1.normalize();

        gsl::Vector3D v2 = a + c;
        v2.normalize();

        gsl::Vector3D v3 = c + b;
        v3.normalize();
        subDivide(a, v1, v2, n - 1);
        subDivide(c, v2, v3, n - 1);
        subDivide(b, v3, v1, n - 1);
        subDivide(v3, v2, v1, n - 1);

    } else {
        makeTriangle(a, b, c);
    }
}

void ResourceManager::makeUnitOctahedron(GLint recursions) {
    gsl::Vector3D v0(0.f, 0.f, 1.f);
    gsl::Vector3D v1(1.f, 0.f, 0.f);
    gsl::Vector3D v2(0.f, 1.f, 0.f);
    gsl::Vector3D v3(-1.f, 0.f, 0.f);
    gsl::Vector3D v4(0.f, -1.f, 0.f);
    gsl::Vector3D v5(0.f, 0.f, -1.f);

    subDivide(v0, v1, v2, recursions);
    subDivide(v0, v2, v3, recursions);
    subDivide(v0, v3, v4, recursions);
    subDivide(v0, v4, v1, recursions);
    subDivide(v5, v2, v1, recursions);
    subDivide(v5, v3, v2, recursions);
    subDivide(v5, v4, v3, recursions);
    subDivide(v5, v1, v4, recursions);
}
