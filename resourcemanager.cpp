#include "resourcemanager.h"
#include "Shaders/colorshader.h"
#include "Shaders/phongshader.h"
#include "Shaders/textureshader.h"
#include "billboard.h"
#include "innpch.h"
#include "lightobject.h"
#include <QDebug>

ResourceManager::ResourceManager() {
}
ResourceManager &ResourceManager::instance() {

    static ResourceManager *mInstance = new ResourceManager();
    return *mInstance;
}

// Do resource manager stuff -- Aka actually delete the pointers after application end
ResourceManager::~ResourceManager() {
    for (auto &shader : Shaders) {
        delete shader.second;
    }
    for (auto &texture : Textures) {
        delete texture.second;
    }
}
Component *ResourceManager::getComponent(CType type, int eID) {
    if (eID <= -1) // This means a eID wasn't given, assumes you want to simply get a component from the last gameobject.
        eID = mGameObjects.size() - 1;
    // If gameobject exists in vector and the component actually exists
    if (eID <= mGameObjects.size() && mGameObjects.at(eID).hasComponent(type)) {
        switch (type) {
        case Transform:
            // Returns a pointer to the transform component. Index is saved in entity -- mComponentsID.at((int)type)
            return &mTransforms.at(mGameObjects.at(eID).mComponentsID.at((int)type));
        case Material:
            return &mMaterials.at(mGameObjects.at(eID).mComponentsID.at((int)type));
        case Mesh:
            return &mMeshes.at(mGameObjects.at(eID).mComponentsID.at((int)type));
        case Light:
            return &mLighting.at(mGameObjects.at(eID).mComponentsID.at((int)type));
        case Input:
            return &mInputs.at(mGameObjects.at(eID).mComponentsID.at((int)type));
        case Physics:
            return &mPhysics.at(mGameObjects.at(eID).mComponentsID.at((int)type));
        case Sound:
            return &mSounds.at(mGameObjects.at(eID).mComponentsID.at((int)type));
        }
    } else {
        qDebug() << "No GameObject with this ID exists.";
    }
    return 0;
}
void ResourceManager::setMesh(MeshComponent *mesh, int eID) {
    if (eID <= -1) // This means a eID wasn't given, assumes you want to simply get a component from the last gameobject.
        eID = mGameObjects.size() - 1;
    // If gameobject exists in vector and the component actually exists
    if (eID <= mGameObjects.size() && mGameObjects.at(eID).hasComponent(Mesh)) {
        mMeshes.at(mGameObjects.at(eID).mComponentsID.at((int)Mesh)) = *mesh;
    }
}
/**
 * @brief ResourceManager::addComponent - Generic component creator
 * The if-check
 * @param type
 * @param eID
 */
void ResourceManager::addComponent(CType type, int eID) {
    if (eID <= -1) // This means a eID wasn't given, assumes you want to simply add to the latest gameobject.
        eID = mGameObjects.size() - 1;

    // If gameobject exists in vector and the component doesn't already exist on object
    if (eID < mGameObjects.size() && !mGameObjects.at(eID).hasComponent(type)) {
        switch (type) {
        case Transform:
            // Not sure about readability here... Transform casts to 0 because that's its enum value.
            // mComponentsID thus holds an index to the last TransformComponent once we add the new component, for easy look-up by eID
            mGameObjects.at(eID).mComponentsID.at((int)Transform) = mTransforms.size();
            mTransforms.emplace_back(TransformComponent());
            break;
        case Material:
            mGameObjects.at(eID).mComponentsID.at((int)Material) = mMaterials.size();
            mMaterials.emplace_back(MaterialComponent());
            break;
        case Mesh:
            mGameObjects.at(eID).mComponentsID.at((int)Mesh) = mMeshes.size();
            // Creates an empty Mesh object -- for use with hardcoded objects mostly.
            mMeshes.emplace_back(MeshComponent());
            break;
        case Light:
            mGameObjects.at(eID).mComponentsID.at((int)Light) = mLighting.size();
            mLighting.emplace_back(LightingComponent());
            break;
        case Input:
            qDebug() << "Use addInputComponent() for now...";
            break;
        case Physics:
            mGameObjects.at(eID).mComponentsID.at((int)Physics) = mPhysics.size();
            mPhysics.emplace_back(PhysicsComponent());
            break;
        case Sound:
            mGameObjects.at(eID).mComponentsID.at((int)Sound) = mSounds.size();
            mSounds.emplace_back(SoundComponent());
            break;
        }
    } else
        qDebug() << "No GameObject with this ID exists.";
}
/**
 * @brief ResourceManager::addMeshComponent - If you don't want a default, empty mesh component,
 * i.e. for prefabs and similar
 * @param name
 * @param eID
 */
void ResourceManager::addMeshComponent(std::string name, int eID) {
    if (eID <= -1 || eID > mGameObjects.size() - 1) {
        eID = mGameObjects.size() - 1;
    }
    addComponent(Mesh, eID);
    setMesh(LoadMesh(name), eID);
}
void ResourceManager::addInputComponent(MainWindow *mainWindow, GLuint eID) {
    if (!mGameObjects.at(eID).hasComponent(Input)) {
        mInputs.emplace_back(InputComponent(mainWindow));
        mGameObjects.at(eID).mComponentsID.at((int)Input) = mInputs.size();
    }
}

GLuint ResourceManager::makeGameObject(std::string name) {
    GLuint eID = mNumObjects;
    ++mNumObjects;
    mGameObjects.emplace_back(GameObject(eID, name));
    return eID;
}
/**
 * @brief ResourceManager::makeXYZ - Creates basic XYZ lines
 */
void ResourceManager::makeXYZ() {
    GLuint eID = makeGameObject("XYZ");
    addComponent(Mesh, eID);
    addComponent(Material, eID);

    initializeOpenGLFunctions();
    mMesh.Clear();
    mMesh.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    mMesh.mVertices.push_back(Vertex{100.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    mMesh.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 1.f, 0.f});
    mMesh.mVertices.push_back(Vertex{0.f, 100.f, 0.f, 0.f, 1.f, 0.f});
    mMesh.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 0.f, 1.f});
    mMesh.mVertices.push_back(Vertex{0.f, 0.f, 100.f, 0.f, 0.f, 1.f});

    // set up buffers (equivalent to init() from before)
    initVertexBuffers();
    // Once VAO and VBO have been generated, mMesh data can be discarded.
    mMaterials.back().setShader(Shaders[Color]);
    mMeshes.back().mVerticeCount = mMesh.mVertices.size();
    mMeshes.back().mDrawType = GL_LINES;

    glBindVertexArray(0);
}

void ResourceManager::makeSkyBox() {
    GLuint eID = makeGameObject("Cube");
    addComponent(Mesh, eID);
    addComponent(Material, eID);

    mMaterials.back().setShader(Shaders[Tex]);
    //    temp->mMatrix.scale(15.f);
    initializeOpenGLFunctions();
    mMesh.Clear();
    mMesh.mVertices.insert(mMesh.mVertices.end(),
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

    mMesh.mIndices.insert(mMesh.mIndices.end(),
                          {
                              0, 2, 1, 1, 2, 3,       //Face 0 - triangle strip (v0,  v1,  v2,  v3)
                              4, 6, 5, 5, 6, 7,       //Face 1 - triangle strip (v4,  v5,  v6,  v7)
                              8, 10, 9, 9, 10, 11,    //Face 2 - triangle strip (v8,  v9, v10,  v11)
                              12, 14, 13, 13, 14, 15, //Face 3 - triangle strip (v12, v13, v14, v15)
                              16, 18, 17, 17, 18, 19, //Face 4 - triangle strip (v16, v17, v18, v19)
                              20, 22, 21, 21, 22, 23  //Face 5 - triangle strip (v20, v21, v22, v23)
                          });
    initVertexBuffers();
    initIndexBuffers();

    mMaterials.back().setTextureUnit(Textures["skybox.bmp"]->id()); // Could just hardcode int 2 here but this seems more readable?
    mMeshes.back().mVerticeCount = mMesh.mVertices.size();
    mMeshes.back().mIndiceCount = mMesh.mIndices.size();
    mMeshes.back().mDrawType = GL_TRIANGLES;

    glBindVertexArray(0);
}

/**
 * @brief ResourceManager::makeTriangleSurface - Reads a .txt file for vertices. Remember to add a material component!
 * @param fileName
 * @return Returns the entity id
 */
GLuint ResourceManager::makeTriangleSurface(std::string fileName) {
    mMesh.Clear();

    initializeOpenGLFunctions();

    GLuint eID = makeGameObject(fileName);
    addComponent(Mesh, eID);
    addComponent(Material, eID);
    setMesh(LoadTriangleMesh(fileName), eID);
    glBindVertexArray(0);

    return eID;
}

void ResourceManager::makeBillBoard() {
    GLuint eID = mNumObjects;
    ++mNumObjects;
    mGameObjects.emplace_back(BillBoard(eID, "BillBoard"));
    addComponent(Mesh, eID);
    addComponent(Material, eID);

    initializeOpenGLFunctions();

    mMesh.Clear();
    mMesh.mVertices.insert(mMesh.mVertices.end(),
                           {
                               // Positions            // Normals          //UVs
                               Vertex{gsl::Vector3D(-2.f, -2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(0.f, 0.f)}, // Bottom Left
                               Vertex{gsl::Vector3D(2.f, -2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(1.f, 0.f)},  // Bottom Right
                               Vertex{gsl::Vector3D(-2.f, 2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(0.f, 1.f)},  // Top Left
                               Vertex{gsl::Vector3D(2.f, 2.f, 0.f), gsl::Vector3D(0.0f, 0.0f, 1.0f), gsl::Vector2D(1.f, 1.f)}    // Top Right
                           });
    mMaterials.back().setTextureUnit(Textures["hund.bmp"]->id());
    mMaterials.back().setShader(Shaders[Tex]);
    mMaterials.back().setColor(gsl::Vector3D(0.7f, 0.6f, 0.1f));

    initVertexBuffers();

    mMeshes.back().mVerticeCount = mMesh.mVertices.size();
    mMeshes.back().mDrawType = GL_TRIANGLE_STRIP;

    glBindVertexArray(0);
}

void ResourceManager::makeOctBall(int n) {
    GLuint eID = makeGameObject("Ball");
    mMesh.Clear();
    initializeOpenGLFunctions();
    addComponent(Mesh, eID);

    GLint mRecursions = n;
    GLint mIndex = 0;

    GLuint mNumberVertices = static_cast<GLuint>(3 * 8 * std::pow(4, mRecursions)); // Not sure what these are used for?

    makeUnitOctahedron(mRecursions);

    initVertexBuffers();
    initIndexBuffers();

    mMeshes.back().mVerticeCount = mMesh.mVertices.size();
    mMeshes.back().mIndiceCount = mMesh.mIndices.size();
    mMeshes.back().mDrawType = GL_TRIANGLES;
    glBindVertexArray(0);
}

void ResourceManager::makeLightObject() {
    GLuint eID = mNumObjects;
    ++mNumObjects;
    mGameObjects.emplace_back(LightObject(eID, "Light"));
    addComponent(Mesh, eID);
    addComponent(Material, eID);

    initializeOpenGLFunctions();

    mMesh.Clear();

    mMesh.mVertices.insert(mMesh.mVertices.end(),
                           {
                               //Vertex data - normals not correct
                               Vertex{gsl::Vector3D(-0.5f, -0.5f, 0.5f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.f, 0.f)},  //Left low
                               Vertex{gsl::Vector3D(0.5f, -0.5f, 0.5f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(1.f, 0.f)},   //Right low
                               Vertex{gsl::Vector3D(0.0f, 0.5f, 0.0f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.5f)},  //Top
                               Vertex{gsl::Vector3D(0.0f, -0.5f, -0.5f), gsl::Vector3D(0.f, 0.f, 1.0f), gsl::Vector2D(0.5f, 0.5f)} //Back low
                           });

    mMesh.mIndices.insert(mMesh.mIndices.end(),
                          {0, 1, 2,
                           1, 3, 2,
                           3, 0, 2,
                           0, 3, 1});
    mMaterials.back().setTextureUnit(Textures["white.bmp"]->id());
    mMaterials.back().setColor(gsl::Vector3D(0.1f, 0.1f, 0.8f));
    mMaterials.back().setShader(Shaders[Tex]);

    initVertexBuffers();
    initIndexBuffers();

    mMeshes.back().mVerticeCount = mMesh.mVertices.size();
    mMeshes.back().mIndiceCount = mMesh.mIndices.size();
    mMeshes.back().mDrawType = GL_TRIANGLES;

    glBindVertexArray(0);
}

void ResourceManager::initVertexBuffers() {

    //Vertex Array Object - VAO
    glGenVertexArrays(1, &mMeshes.back().mVAO);
    glBindVertexArray(mMeshes.back().mVAO);

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers(1, &mMeshes.back().mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mMeshes.back().mVBO);

    glBufferData(GL_ARRAY_BUFFER, mMesh.mVertices.size() * sizeof(Vertex), mMesh.mVertices.data(), GL_STATIC_DRAW);

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

void ResourceManager::initIndexBuffers() {
    glGenBuffers(1, &mMeshes.back().mEAB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mMeshes.back().mEAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mMesh.mIndices.size() * sizeof(GLuint), mMesh.mIndices.data(), GL_STATIC_DRAW);
}

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
 * @brief ResourceManager::GetMesh - Use this function if you know you've already loaded the mesh.
 * @param name
 * @return
 */
MeshComponent *ResourceManager::GetMesh(std::string name) {
    return &mMeshes.at(mMeshMap[name]);
}
bool ResourceManager::readFile(std::string fileName) {
    //Open File
    std::string fileWithPath = gsl::assetFilePath + "Meshes/" + fileName;
    std::ifstream fileIn;
    fileIn.open(fileWithPath, std::ifstream::in);
    if (!fileIn) {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(fileName);
        return false;
    }

    mMesh.Clear();

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
                    mMesh.mVertices.push_back(tempVert);
                } else //no uv in mesh data, use 0, 0 as uv
                {
                    Vertex tempVert(tempVertecies[index], tempNormals[normal], gsl::Vector2D(0.0f, 0.0f));
                    mMesh.mVertices.push_back(tempVert);
                }
                mMesh.mIndices.push_back(temp_index++);
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

    initVertexBuffers();
    initIndexBuffers();

    mMeshes.back().mVerticeCount = mMesh.mVertices.size();
    mMeshes.back().mIndiceCount = mMesh.mIndices.size();
    mMeshes.back().mDrawType = GL_TRIANGLES;

    qDebug() << "Obj file read: " << QString::fromStdString(fileName);
    return true;
}

/**
 * @brief ResourceManager::LoadMesh - Loads the mesh from file if it isn't already in the Meshes map.
 * @param fileName
 * @return
 */
MeshComponent *ResourceManager::LoadMesh(std::string fileName) {
    auto search = mMeshMap.find(fileName);
    if (search != mMeshMap.end()) {
        return &mMeshes[search->second]; // Return a copy of the mesh it wants if already stored
    }
    if (!readFile(fileName)) { // Should run readFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return 0;
    }
    mMeshMap[fileName] = mMeshes.size() - 1; // MeshComponent has already been added to the vector, subtract size() by 1 to get the last index.
    return &mMeshes.back();                  // the mesh at the back is the latest creation
}
MeshComponent *ResourceManager::LoadTriangleMesh(std::string fileName) {
    auto search = mMeshMap.find(fileName);
    if (search != mMeshMap.end()) {
        return &mMeshes[search->second]; // Return a copy of the mesh it wants if already stored
    }
    if (!readTriangleFile(fileName)) { // Should run readTriangleFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return 0;
    }
    mMeshMap[fileName] = mMeshes.size() - 1; // MeshComponent has already been added to the vector, subtract size() by 1 to get the last index.
    return &mMeshes.back();                  // the mesh at the back is the latest creation
}

bool ResourceManager::readTriangleFile(std::string fileName) {
    std::ifstream inn;
    std::string fileWithPath = gsl::assetFilePath + "Meshes/" + fileName;
    mMesh.Clear();
    inn.open(fileWithPath);

    if (inn.is_open()) {
        int n;
        Vertex vertex;
        inn >> n;

        mMesh.mVertices.reserve(n);
        for (int i = 0; i < n; i++) {
            inn >> vertex;
            mMesh.mVertices.push_back(vertex);
        }
        inn.close();
        qDebug() << "TriangleSurface file read: " << QString::fromStdString(fileName);

        initVertexBuffers();

        mMeshes.back().mVerticeCount = mMesh.mVertices.size();
        mMeshes.back().mDrawType = GL_TRIANGLES;
        return true;
    } else {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(fileName);
        return false;
    }
}
void ResourceManager::makeTriangle(const gsl::Vector3D &v1, const gsl::Vector3D &v2, const gsl::Vector3D &v3) {
    mMesh.mVertices.push_back(Vertex(v1, v1, gsl::Vector2D(0.f, 0.f)));
    mMesh.mVertices.push_back(Vertex(v2, v2, gsl::Vector2D(1.f, 0.f)));
    mMesh.mVertices.push_back(Vertex(v3, v3, gsl::Vector2D(0.5f, 1.f)));
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
