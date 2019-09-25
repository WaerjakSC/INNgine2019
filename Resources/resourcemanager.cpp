#include "resourcemanager.h"
#include "billboard.h"
#include "colorshader.h"
#include "innpch.h"
#include "lightsystem.h"
#include "phongshader.h"
#include "registry.h"
#include "renderview.h"
#include "textureshader.h"
#include <QDebug>

ResourceManager *ResourceManager::mInstance = nullptr;

ResourceManager::ResourceManager() {
    registry = Registry::instance();
    //    registry->registerComponent<Transform>();
    //    registry->registerComponent<Material>();
    //    registry->registerComponent<Mesh>();
    //    registry->registerComponent<Light>();
    // Move these elsewhere once they have systems
    registry->registerComponent<Input>();
    registry->registerComponent<Physics>();
    registry->registerComponent<Sound>();

    // Beware of which class is created first - If ResourceManager is created first and starts making objects, it needs to register component types first.
    // On the other hand, if the systems are created first then you probably won't need to register anything in here, since those systems should take care of it.
    // Initialize shared pointers
    //    mTransforms = std::make_shared<Pool<TransformComponent>>();

    // Set up Views
    //    mRenderView = std::make_unique<RenderView>(mTransforms);
}

// Do resource manager stuff -- Aka actually delete the pointers after application end
ResourceManager::~ResourceManager() {
    for (auto &shader : Shaders) {
        delete shader.second;
    }
    for (auto &texture : Textures) {
        delete texture.second;
    }
    for (auto &entity : mGameObjects)
        delete entity;
}
ResourceManager *ResourceManager::instance() {
    if (!mInstance)
        mInstance = new ResourceManager();
    return mInstance;
}

/**
 * @brief If you know the mesh you want at construction i.e. for prefabs and similar
 * @param name - name of the file you want to read
 * @param eID - entityID
 */
void ResourceManager::addMeshComponent(std::string name, int eID) {
    if (eID <= -1 || (size_t)eID > mGameObjects.size() - 1) {
        eID = mGameObjects.size() - 1;
    }
    registry->addComponent<Mesh>(eID);
    setMesh(name, eID);
}
void ResourceManager::setMesh(Mesh *mesh, int eID) {
    // If gameobject exists in vector and the component actually exists
    if ((size_t)eID < mGameObjects.size()) {
        registry->getComponent<Mesh>(eID) = *mesh;
    }
}
void ResourceManager::setMesh(std::string name, int eID) {
    auto search = mMeshMap.find(name);
    if (search != mMeshMap.end()) {
        registry->getComponent<Mesh>(eID) = search->second;
    } else
        LoadMesh(name);
}

/**
 * @brief Get a pointer to the entity with the specified ID.
 * @param eID
 * @return
 */
GameObject *ResourceManager::getGameObject(int eID) {
    assert((size_t)eID < mGameObjectIndex.size());
    return mGameObjects.at(mGameObjectIndex.at(eID));
}
/**
 * @brief Destroy gameobject
 * @param eID - entityID
 */
void ResourceManager::removeGameObject(int eID) {
    assert((size_t)eID < mGameObjectIndex.size());
    mGameObjectIndex.at(mGameObjects.back()->eID) = mGameObjectIndex.at(eID); // Set the index to point to the location after swap
    std::swap(mGameObjects.at(mGameObjectIndex[eID]), mGameObjects.back());   // Swap the removed with the last, then pop out the last.
    mGameObjects.pop_back();
    mGameObjectIndex.at(eID) = -1;  // Set entity location to an invalid value.
    registry->entityDestroyed(eID); // Pass the message on to the registry
}
/**
 * @brief Make a generic game object with no components attached.
 * @param name Name of the gameobject. Leave blank if no name desired.
 * @return Returns the entity ID for use in adding components or other tasks.
 */
GLuint ResourceManager::makeGameObject(std::string name) {
    GLuint eID = mNumGameObjects;
    mNumGameObjects++;
    mGameObjectIndex.emplace_back(mGameObjects.size());
    mGameObjects.emplace_back(new GameObject(eID, name));

    return eID;
}
/**
 * @brief Make a standard 3D object from a .obj or .txt file with the given name and type
 * @param name
 * @param type
 * @return The entity ID of the gameobject.
 */
GLuint ResourceManager::make3DObject(std::string name, ShaderType type) {
    GLuint eID = makeGameObject(name);
    registry->addComponent<Transform>(eID);
    registry->addComponent<Material>(eID, type);
    addMeshComponent(name, eID);
    return eID;
}
/**
 * @brief Plane prefab -- should fix coloring at some point
 * @return
 */
GLuint ResourceManager::makePlane() {
    GLuint eID = makeGameObject("Plane");
    registry->addComponent<Transform>(eID);
    registry->addComponent<Material>(eID, Color);

    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mVertices.push_back(Vertex{-0.5f, 0.f, -0.5f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.5f, 0.f, 0.5f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.5f, 0.f, -0.5f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.5f, 0.f, 0.5f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{-0.5f, 0.f, -0.5f, 0.f, 0.f, 1.f});
    mMeshData.mVertices.push_back(Vertex{-0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f});
    registry->addComponent<Mesh>(eID, GL_TRIANGLES, mMeshData);

    // Once VAO and VBO have been generated, mMeshData can be discarded.
    auto &mesh = registry->getComponent<Mesh>(eID);

    // set up buffers (equivalent to init() from before)
    initVertexBuffers(&mesh);
    glBindVertexArray(0);
    return eID;
}
/**
 * @brief Cube prefab
 * @return
 */
GLuint ResourceManager::makeCube() {
    GLuint eID = makeGameObject("Cube");
    registry->addComponent<Transform>(eID);
    registry->addComponent<Material>(eID, Color);
    registry->addComponent<Mesh>(eID);
    setMesh("cube.obj", eID);

    return eID;
}
/**
 * @brief Creates basic XYZ lines
 */
GLuint ResourceManager::makeXYZ() {
    GLuint eID = makeGameObject("XYZ");
    registry->addComponent<Transform>(eID);
    registry->addComponent<Material>(eID, Color);

    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{100.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 100.f, 0.f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 0.f, 1.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 100.f, 0.f, 0.f, 1.f});
    registry->addComponent<Mesh>(eID, GL_LINES, mMeshData);

    // Once VAO and VBO have been generated, mMesh data can be discarded.
    auto &mesh = registry->getComponent<Mesh>(eID);

    // set up buffers (equivalent to init() from before)
    initVertexBuffers(&mesh);
    glBindVertexArray(0);
    return eID;
}
/**
 * @brief Prefab skybox for editor
 * @return
 */
GLuint ResourceManager::makeSkyBox() {
    GLuint eID = makeGameObject("Skybox");
    registry->addComponent<Transform>(eID, 0, 0, gsl::Vector3D(15));
    registry->addComponent<Material>(eID, Tex, Textures["skybox.bmp"]->id() - 1);

    //    temp->mMatrix.scale(15.f);
    initializeOpenGLFunctions();
    mMeshData.Clear();
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
    registry->addComponent<Mesh>(eID, GL_TRIANGLES, mMeshData); // If using meshData struct, remember to add the Mesh component AFTER clearing and inserting mMeshData

    //    skyMat.setTextureUnit(Textures["skybox.bmp"]->id() - 1); // Not sure why the ID is one ahead of the actual texture I want??
    auto &skyMesh = registry->getComponent<Mesh>(eID);
    initVertexBuffers(&skyMesh);
    initIndexBuffers(&skyMesh);

    glBindVertexArray(0);

    return eID;
}

/**
 * @brief Reads a .txt file for vertices.
 * @param fileName
 * @return Returns the entity id
 */
GLuint ResourceManager::makeTriangleSurface(std::string fileName) {
    GLuint eID = makeGameObject(fileName);

    mMeshData.Clear();

    initializeOpenGLFunctions();

    registry->addComponent<Transform>(eID);
    registry->addComponent<Material>(eID);
    registry->addComponent<Mesh>(eID);
    setMesh(LoadTriangleMesh(fileName), eID);
    glBindVertexArray(0);

    return eID;
}
/**
 * @brief Billboard prefab
 * @return
 */
GLuint ResourceManager::makeBillBoard() {
    GLuint eID = mNumGameObjects;
    ++mNumGameObjects;
    mGameObjectIndex.emplace_back(mGameObjects.size());
    mGameObjects.emplace_back(new BillBoard(eID, "BillBoard"));
    registry->addComponent<Transform>(eID, gsl::Vector3D(4.f, 0.f, -3.5f));

    registry->addComponent<Material>(eID, Tex, Textures["gnome.bmp"]->id() - 1);

    initializeOpenGLFunctions();

    mMeshData.Clear();
    mMeshData.mVertices.insert(mMeshData.mVertices.end(),
                               {
                                   // Positions            // Normals          //UVs
                                   Vertex{gsl::Vector3D(-2.f, -2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(0.f, 0.f)}, // Bottom Left
                                   Vertex{gsl::Vector3D(2.f, -2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(1.f, 0.f)},  // Bottom Right
                                   Vertex{gsl::Vector3D(-2.f, 2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(0.f, 1.f)},  // Top Left
                                   Vertex{gsl::Vector3D(2.f, 2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(1.f, 1.f)}    // Top Right
                               });
    registry->addComponent<Mesh>(eID, GL_TRIANGLE_STRIP, mMeshData);

    auto &billBoardMesh = registry->getComponent<Mesh>(eID);
    initVertexBuffers(&billBoardMesh);

    glBindVertexArray(0);
    return eID;
}
/**
 * @brief Sphere prefab
 * @param n - number of recursions. Increase number for "rounder" sphere
 * @return
 */
GLuint ResourceManager::makeOctBall(int n) {
    GLuint eID = makeGameObject("Ball");
    mMeshData.Clear();
    initializeOpenGLFunctions();
    registry->addComponent<Transform>(eID);
    registry->addComponent<Material>(eID, Color);

    GLint mRecursions = n;
    GLint mIndex = 0;

    GLuint mNumberVertices = static_cast<GLuint>(3 * 8 * std::pow(4, mRecursions)); // Not sure what these are used for?

    makeUnitOctahedron(mRecursions); // This fills mMeshData

    registry->addComponent<Mesh>(eID, GL_TRIANGLES, mMeshData);
    auto &octMesh = registry->getComponent<Mesh>(eID);

    initVertexBuffers(&octMesh);
    initIndexBuffers(&octMesh);

    glBindVertexArray(0);
    return eID;
}
/**
 * @brief Light object prefab -- not fully implemented yet
 * @return
 */
GLuint ResourceManager::makeLightObject() {
    GLuint eID = makeGameObject("Light");
    registry->addComponent<Transform>(eID, gsl::Vector3D(2.5f, 3.f, 0.f), gsl::Vector3D(0.0f, 180.f, 0.0f));
    registry->addComponent<Material>(eID, Tex, Textures["white.bmp"]->id() - 1, gsl::Vector3D(0.1f, 0.1f, 0.8f));
    registry->addComponent<Light>(eID);

    initializeOpenGLFunctions();

    mMeshData.Clear();

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
    registry->addComponent<Mesh>(eID, GL_TRIANGLES, mMeshData);

    auto &lightMesh = registry->getComponent<Mesh>(eID);

    initVertexBuffers(&lightMesh);
    initIndexBuffers(&lightMesh);

    glBindVertexArray(0);
    return eID;
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
 * @brief Load shader for the first time if it's not already in storage.
 * @param type
 * @param geometryPath
 */
void ResourceManager::LoadShader(ShaderType type, const GLchar *geometryPath) {
    if (Shaders.find(type) == Shaders.end()) {
        std::string shaderName;
        switch (type) {
        case ShaderType::Color:
            shaderName = "plainshader";
            Shaders[type] = new ColorShader(shaderName, geometryPath);
            break;
        case ShaderType::Tex:
            shaderName = "textureshader";
            Shaders[type] = new TextureShader(shaderName, geometryPath);
            break;
        case ShaderType::Phong:
            shaderName = "phongshader";
            Shaders[type] = new PhongShader(shaderName, geometryPath);
            break;
        default:
            qDebug() << "Failed to find shader in switch statement";
        }
        qDebug() << "ResourceManager: Added shader " << QString::fromStdString(shaderName);
    } else {
        qDebug() << "ResourceManager: Shader already loaded, ignoring...";
    }
}
/**
 * @brief Load texture if it's not already in storage.
 * @param fileName
 * @param textureUnit
 */
void ResourceManager::LoadTexture(std::string fileName, GLuint textureUnit) {
    if (Textures.find(fileName) == Textures.end()) {
        Textures[fileName] = new Texture(fileName, textureUnit);

        qDebug() << "ResourceManager: Added texture" << QString::fromStdString(fileName);
    }
}

Shader *ResourceManager::GetShader(ShaderType type) {
    return Shaders[type];
}

Texture *ResourceManager::GetTexture(std::string fileName) {
    return Textures[fileName];
}
/**
 * @brief Read .obj file.
 * @param fileName
 * @return
 */
bool ResourceManager::readFile(std::string fileName) {
    //Open File
    std::string fileWithPath = gsl::assetFilePath + "Meshes/" + fileName;
    std::ifstream fileIn;
    fileIn.open(fileWithPath, std::ifstream::in);
    if (!fileIn) {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(fileName);
        return false;
    }

    mMeshData.Clear();

    //One line at a time-variable
    std::string oneLine;
    //One word at a time-variable
    std::string oneWord;

    std::vector<gsl::Vector3D> tempVertecies;
    std::vector<gsl::Vector3D> tempNormals;
    std::vector<gsl::Vector2D> tempUVs;

    // Variable for constructing the indices vector
    unsigned int temp_index = 0;

    //Reading one line at a time from file to oneLine
    while (std::getline(fileIn, oneLine)) {
        //Doing a trick to get one word at a time
        std::stringstream sStream;
        //Pushing line into stream
        sStream << oneLine;
        //Streaming one word out of line
        oneWord = ""; //resetting the value or else the last value might survive!
        sStream >> oneWord;

        if (oneWord == "#") {
            //Ignore this line
            //            qDebug() << "Line is comment "  << QString::fromStdString(oneWord);
            continue;
        }
        if (oneWord == "") {
            //Ignore this line
            //            qDebug() << "Line is blank ";
            continue;
        }
        if (oneWord == "v") {
            //            qDebug() << "Line is vertex "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector3D tempVertex;
            sStream >> oneWord;
            tempVertex.x = std::stof(oneWord);
            sStream >> oneWord;
            tempVertex.y = std::stof(oneWord);
            sStream >> oneWord;
            tempVertex.z = std::stof(oneWord);

            //Vertex made - pushing it into vertex-vector
            tempVertecies.push_back(tempVertex);
            continue;
        }
        if (oneWord == "vt") {
            //            qDebug() << "Line is UV-coordinate "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector2D tempUV;
            sStream >> oneWord;
            tempUV.x = std::stof(oneWord);
            sStream >> oneWord;
            tempUV.y = std::stof(oneWord);

            //UV made - pushing it into UV-vector
            tempUVs.push_back(tempUV);

            continue;
        }
        if (oneWord == "vn") {
            //            qDebug() << "Line is normal "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector3D tempNormal;
            sStream >> oneWord;
            tempNormal.x = std::stof(oneWord);
            sStream >> oneWord;
            tempNormal.y = std::stof(oneWord);
            sStream >> oneWord;
            tempNormal.z = std::stof(oneWord);

            //Vertex made - pushing it into vertex-vector
            tempNormals.push_back(tempNormal);
            continue;
        }
        if (oneWord == "f") {
            //            qDebug() << "Line is a face "  << QString::fromStdString(oneWord) << " ";
            //int slash; //used to get the / from the v/t/n - format
            int index, normal, uv;
            for (int i = 0; i < 3; i++) {
                sStream >> oneWord;                          //one word read
                std::stringstream tempWord(oneWord);         //to use getline on this one word
                std::string segment;                         //the numbers in the f-line
                std::vector<std::string> segmentArray;       //temp array of the numbers
                while (std::getline(tempWord, segment, '/')) //splitting word in segments
                {
                    segmentArray.push_back(segment);
                }
                index = std::stoi(segmentArray[0]); //first is vertex
                if (segmentArray[1] != "")          //second is uv
                    uv = std::stoi(segmentArray[1]);
                else {
                    //qDebug() << "No uvs in mesh";       //uv not present
                    uv = 0; //this will become -1 in a couple of lines
                }
                normal = std::stoi(segmentArray[2]); //third is normal

                //Fixing the indexes
                //because obj f-lines starts with 1, not 0
                --index;
                --uv;
                --normal;

                if (uv > -1) //uv present!
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], tempUVs[uv]);
                    mMeshData.mVertices.push_back(tempVert);
                } else //no uv in mesh data, use 0, 0 as uv
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], gsl::Vector2D(0.0f, 0.0f));
                    mMeshData.mVertices.push_back(tempVert);
                }
                mMeshData.mIndices.push_back(temp_index++);
            }

            //For some reason the winding order is backwards so fixing this by swapping the last two indices
            //Update: this was because the matrix library was wrong - now it is corrected so this is no longer needed.
            //            unsigned int back = mIndices.size() - 1;
            //            std::swap(mIndices.at(back), mIndices.at(back-1));
            continue;
        }
    }

    //being a nice boy and closing the file after use
    fileIn.close();

    auto &mesh = registry->getLastComponent<Mesh>();
    mesh.mVerticeCount = mMeshData.mVertices.size();
    mesh.mIndiceCount = mMeshData.mIndices.size();
    mesh.mDrawType = GL_TRIANGLES;

    initVertexBuffers(&mesh);
    initIndexBuffers(&mesh);

    qDebug() << "Obj file read: " << QString::fromStdString(fileName);
    return true;
}

/**
 * @brief ResourceManager::LoadMesh - Loads the mesh from file if it isn't already in the Meshes map.
 * @param fileName
 * @return
 */
void ResourceManager::LoadMesh(std::string fileName) {

    //    auto search = mMeshMap.find(fileName);
    //    if (search != mMeshMap.end()) {
    //        return &search->second; // Return a copy of the mesh it wants if already stored
    //    }
    if (!readFile(fileName)) { // Should run readFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return;
    }
    // the mesh at the back is the latest creation
    mMeshMap[fileName] = registry->getLastComponent<Mesh>(); // Save the new unique mesh in the meshmap for other entities that might need it
}

Mesh *ResourceManager::LoadTriangleMesh(std::string fileName) {
    auto search = mMeshMap.find(fileName);
    if (search != mMeshMap.end()) {
        return &search->second; // Return a copy of the mesh it wants if already stored
    }
    if (!readTriangleFile(fileName)) { // Should run readTriangleFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return new Mesh;
    }
    mMeshMap[fileName] = registry->getLastComponent<Mesh>(); // Save the new unique mesh in the meshmap for other entities that might need it
    return &registry->getLastComponent<Mesh>();              // the mesh at the back is the latest creation
}

bool ResourceManager::readTriangleFile(std::string fileName) {
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

        auto &mesh = registry->getLastComponent<Mesh>();
        mesh.mVerticeCount = mMeshData.mVertices.size();
        mesh.mDrawType = GL_TRIANGLES;
        initVertexBuffers(&mesh);
        return true;
    } else {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(fileName);
        return false;
    }
}
void ResourceManager::setLightSystem(const std::shared_ptr<LightSystem> &lightSystem) {
    mLightSystem = lightSystem;
}

/**
 * @brief Get a raw pointer to mRenderView -- ResourceManager owns this as a unique_ptr
 * @return
 */
//RenderView *ResourceManager::getRenderView() const {
//    return mRenderView.get();
//}

std::vector<int> ResourceManager::getGameObjectIndex() const {
    return mGameObjectIndex;
}

std::vector<GameObject *> ResourceManager::getGameObjects() const {
    return mGameObjects;
}

std::map<ShaderType, Shader *> ResourceManager::getShaders() const {
    return Shaders;
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
