#include "resourcemanager.h"
#include "billboard.h"
#include "colorshader.h"
#include "innpch.h"
#include "lightsystem.h"
#include "meshcomponent.h"
#include "phongshader.h"
#include "renderview.h"
#include "textureshader.h"
#include <QDebug>

ResourceManager *ResourceManager::mInstance = nullptr;

ResourceManager::ResourceManager() {
    // Initialize shared pointers
    mTransforms = std::make_shared<Pool<TransformComponent>>();

    // Set up Views
    mRenderView = std::make_unique<RenderView>(mTransforms);
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
 * @brief ResourceManager::removeComponent - Remove a component and update tables.
 * @param type Type of component to remove
 * @param eID entity ID
 */
void ResourceManager::removeComponent(CType type, int eID) {
    if (eID <= -1) // This means a eID wasn't given, assumes you want to simply add to the latest gameobject.
        eID = mGameObjects.size() - 1;
    // If gameobject exists in vector and the component doesn't already exist on object
    if ((size_t)eID < mGameObjects.size()) {
        switch (type) {
        case Transform:
            mTransforms->remove(eID);
            break;
        case Material:
            matPool()->remove(eID);
            break;
        case Mesh:
            meshPool()->remove(eID);
            break;
        case Light:
            lightPool()->remove(eID);
            break;
        case Input:
            mInputs.remove(eID);
            break;
        case Physics:
            mPhysics.remove(eID);
            break;
        case Sound:
            mSounds.remove(eID);
            break;
        default:
            qDebug() << "Unknown Type.";
        }
    } else
        qDebug() << "No GameObject with this ID exists or Component already exists.";
}
Component *ResourceManager::getComponent(CType type, int eID) {
    if (eID <= -1) // This means an eID wasn't given, assumes you want to simply get a component from the last gameobject.
        eID = mGameObjects.size() - 1;
    // If gameobject exists in vector and the component actually exists
    if ((size_t)eID < mGameObjects.size())
        switch (type) {
        case Transform:
            return mTransforms->get(eID);
        case Material:
            return matPool()->get(eID);
        case Mesh:
            return meshPool()->get(eID);
        case Light:
            return lightPool()->get(eID);
        case Input:
            return mInputs.get(eID);
        case Physics:
            return mPhysics.get(eID);
        case Sound:
            return mSounds.get(eID);
        default:
            qDebug() << "Unknown Type.";
        }
    else {
        qDebug() << "No GameObject with this ID exists.";
        return 0;
    }
    return 0;
}
/**
 * @brief Must be updated manually if a new component type is added.
 * Returns all components owned by the entity.
 * Use dynamic casts to access the actual component types.
 * @param eID
 * @return
 */
std::vector<Component *> ResourceManager::getComponents(int eID) {
    std::vector<Component *> components;
    if (mTransforms->has(eID))
        components.emplace_back(mTransforms->get(eID));
    if (matPool()->has(eID))
        components.emplace_back(matPool()->get(eID));
    if (meshPool()->has(eID))
        components.emplace_back(meshPool()->get(eID));
    if (lightPool()->has(eID))
        components.emplace_back(lightPool()->get(eID));
    if (mInputs.has(eID))
        components.emplace_back(mInputs.get(eID));
    if (mPhysics.has(eID))
        components.emplace_back(mPhysics.get(eID));
    if (mSounds.has(eID))
        components.emplace_back(mSounds.get(eID));
    return components;
}
/**
 * @brief ResourceManager::addComponent - Generic component creator
 * @param type Component type enum
 * @param eID Entity ID
 */
void ResourceManager::addComponent(CType type, int eID) {
    if (eID <= -1) // This means a eID wasn't given, assumes you want to simply add to the latest gameobject.
        eID = mGameObjects.size() - 1;
    // If gameobject exists in vector and the component doesn't already exist on object
    if ((size_t)eID < mGameObjects.size()) {
        switch (type) {
        case Transform:
            mTransforms->add(eID);
            break;
        case Material:
            matPool()->add(eID);
            break;
        case Mesh:
            // Creates an empty Mesh object -- for use with hardcoded objects mostly.
            meshPool()->add(eID);
            break;
        case Light:
            lightPool()->add(eID);
            break;
        case Input:
            mInputs.add(eID);
            break;
        case Physics:
            mPhysics.add(eID);
            break;
        case Sound:
            mSounds.add(eID);
            break;
        }
    } else
        qDebug() << "No GameObject with this ID exists or Component already exists.";
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
    addComponent(Mesh, eID);
    setMesh(name, eID);
}
void ResourceManager::setMesh(MeshComponent *mesh, int eID) {
    // If gameobject exists in vector and the component actually exists
    if ((size_t)eID < mGameObjects.size()) {
        meshPool()->get(eID)->copyOpenGLData(*mesh);
    }
}
void ResourceManager::setMesh(std::string name, int eID) {
    auto search = mMeshMap.find(name);
    if (search != mMeshMap.end()) {
        meshPool()->get(eID)->copyOpenGLData(search->second);
    } else
        LoadMesh(name);
}
/**
 * @brief Set the parent of a gameobject (or rather its transform component).
 * Note: Currently no support for setting an item to be a child of a previously created item, due to how we're currently inserting into items into the view.
 * For now, make sure you create items in the order you want them to be parented, i.e. Parent first, then Children.
 * @param eID
 * @param parentID
 */
void ResourceManager::setParent(int eID, int parentID) {
    if (mTransforms->get(eID)->parentID != -1) // Make sure to remove the child from its old parent if it had one
        mTransforms->get(mTransforms->get(eID)->parentID)->removeChild(eID);
    mTransforms->get(eID)->parentID = parentID; // Set the new parent ID. Can be set to -1 if you want it to be independent again.
    if (parentID != -1)
        mTransforms->get(parentID)->addChild(eID);
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
 * @brief TODO: make sure to tell all Pools to remove this gameobject from their lists.
 * @param eID - entityID
 */
void ResourceManager::removeGameObject(int eID) {
    assert((size_t)eID < mGameObjectIndex.size());
    mGameObjectIndex.at(mGameObjects.back()->eID) = mGameObjectIndex.at(eID); // Set the index to point to the location after swap
    std::swap(mGameObjects.at(mGameObjectIndex[eID]), mGameObjects.back());   // Swap the removed with the last, then pop out the last.
    mGameObjects.pop_back();
    mGameObjectIndex.at(eID) = -1; // Set entity location to an invalid value.
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
    addComponent(Transform);
    addComponent(Material);
    addMeshComponent(name, eID);
    matPool()->get(eID)->setShader(type);
    return eID;
}
/**
 * @brief Plane prefab -- should fix coloring at some point
 * @return
 */
GLuint ResourceManager::makePlane() {
    GLuint eID = makeGameObject("Plane");
    addComponent(Transform);
    addComponent(Material);
    addComponent(Mesh);

    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mVertices.push_back(Vertex{-0.5f, 0.f, -0.5f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.5f, 0.f, 0.5f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.5f, 0.f, -0.5f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.5f, 0.f, 0.5f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{-0.5f, 0.f, -0.5f, 0.f, 0.f, 1.f});
    mMeshData.mVertices.push_back(Vertex{-0.5f, 0.f, 0.5f, 0.f, 0.f, 1.f});

    // Once VAO and VBO have been generated, mMesh data can be discarded.
    matPool()->get(eID)->setShader(Color);
    meshPool()->get(eID)->mVerticeCount = mMeshData.mVertices.size();
    meshPool()->get(eID)->mDrawType = GL_TRIANGLES;

    // set up buffers (equivalent to init() from before)
    initVertexBuffers(meshPool()->get(eID));
    glBindVertexArray(0);
    return eID;
}
/**
 * @brief Cube prefab
 * @return
 */
GLuint ResourceManager::makeCube() {
    GLuint eID = makeGameObject("Cube");
    addComponent(Transform);
    addComponent(Material);
    addComponent(Mesh);
    setMesh("cube.obj", eID);

    // Once VAO and VBO have been generated, mMesh data can be discarded.
    matPool()->get(eID)->setShader(Color);

    return eID;
}
/**
 * @brief Creates basic XYZ lines
 */
GLuint ResourceManager::makeXYZ() {
    GLuint eID = makeGameObject("XYZ");
    addComponent(Transform);
    addComponent(Material);
    addComponent(Mesh);

    initializeOpenGLFunctions();
    mMeshData.Clear();
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{100.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 100.f, 0.f, 0.f, 1.f, 0.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 0.f, 1.f});
    mMeshData.mVertices.push_back(Vertex{0.f, 0.f, 100.f, 0.f, 0.f, 1.f});

    // Once VAO and VBO have been generated, mMesh data can be discarded.
    matPool()->get(eID)->setShader(Color);
    meshPool()->get(eID)->mVerticeCount = mMeshData.mVertices.size();
    meshPool()->get(eID)->mDrawType = GL_LINES;

    // set up buffers (equivalent to init() from before)
    initVertexBuffers(meshPool()->get(eID));
    glBindVertexArray(0);
    return eID;
}
/**
 * @brief Prefab skybox for editor
 * @return
 */
GLuint ResourceManager::makeSkyBox() {
    GLuint eID = makeGameObject("Skybox");
    addComponent(Transform);
    addComponent(Material);
    addComponent(Mesh);

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

    MaterialComponent *skyMat = matPool()->get(eID);
    skyMat->setShader(Tex);

    skyMat->setTextureUnit(Textures["skybox.bmp"]->id() - 1); // Not sure why the ID is one ahead of the actual texture I want??
    MeshComponent *skyMesh = meshPool()->get(eID);
    skyMesh->mVerticeCount = mMeshData.mVertices.size();
    skyMesh->mIndiceCount = mMeshData.mIndices.size();
    skyMesh->mDrawType = GL_TRIANGLES;

    initVertexBuffers(skyMesh);
    initIndexBuffers(skyMesh);

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

    addComponent(Transform);
    addComponent(Material);
    addComponent(Mesh);
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
    addComponent(Transform);
    addComponent(Material);
    addComponent(Mesh);

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
    MaterialComponent *billBoardMat = matPool()->get(eID);
    billBoardMat->setTextureUnit(Textures["gnome.bmp"]->id() - 1);
    billBoardMat->setShader(Tex);
    billBoardMat->setColor(gsl::Vector3D(1.0f, 1.0f, 1.0f));

    MeshComponent *billBoardMesh = meshPool()->get(eID);
    billBoardMesh->mVerticeCount = mMeshData.mVertices.size();
    billBoardMesh->mDrawType = GL_TRIANGLE_STRIP;

    initVertexBuffers(billBoardMesh);

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
    addComponent(Transform, eID);
    addComponent(Material, eID);
    addComponent(Mesh, eID);

    GLint mRecursions = n;
    GLint mIndex = 0;

    GLuint mNumberVertices = static_cast<GLuint>(3 * 8 * std::pow(4, mRecursions)); // Not sure what these are used for?

    makeUnitOctahedron(mRecursions);

    //    mTransforms.get(eID)->matrix().scale(0.5f, 0.5f, 0.5f);
    MeshComponent *OctMesh = meshPool()->get(eID);
    OctMesh->mVerticeCount = mMeshData.mVertices.size();
    OctMesh->mIndiceCount = mMeshData.mIndices.size();
    OctMesh->mDrawType = GL_TRIANGLES;
    matPool()->get(eID)->setShader(Color);

    initVertexBuffers(OctMesh);
    initIndexBuffers(OctMesh);

    glBindVertexArray(0);
    return eID;
}
/**
 * @brief Light object prefab -- not fully implemented yet
 * @return
 */
GLuint ResourceManager::makeLightObject() {
    GLuint eID = makeGameObject("Light");
    addComponent(Transform);
    addComponent(Material);
    addComponent(Mesh);
    addComponent(Light);

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

    MaterialComponent *lightMat = matPool()->get(eID);
    lightMat->setTextureUnit(Textures["white.bmp"]->id() - 1);
    lightMat->setColor(gsl::Vector3D(0.1f, 0.1f, 0.8f));
    lightMat->setShader(Tex);

    MeshComponent *lightMesh = meshPool()->get(eID);
    lightMesh->mVerticeCount = mMeshData.mVertices.size();
    lightMesh->mIndiceCount = mMeshData.mIndices.size();
    lightMesh->mDrawType = GL_TRIANGLES;

    initVertexBuffers(lightMesh);
    initIndexBuffers(lightMesh);

    glBindVertexArray(0);
    return eID;
}
/**
 * @brief opengl init - initialize the given mesh's buffers and arrays
 */
void ResourceManager::initVertexBuffers(MeshComponent *mesh) {
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
void ResourceManager::initIndexBuffers(MeshComponent *mesh) {
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

    MeshComponent *mesh = meshPool()->getLast();
    mesh->mVerticeCount = mMeshData.mVertices.size();
    mesh->mIndiceCount = mMeshData.mIndices.size();
    mesh->mDrawType = GL_TRIANGLES;

    initVertexBuffers(mesh);
    initIndexBuffers(mesh);

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
        return &search->second; // Return a copy of the mesh it wants if already stored
    }
    if (!readFile(fileName)) { // Should run readFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return 0;
    }
    mMeshMap[fileName] = *meshPool()->getLast(); // Save the new unique mesh in the meshmap for other entities that might need it
    return meshPool()->getLast();                // the mesh at the back is the latest creation
}

MeshComponent *ResourceManager::LoadTriangleMesh(std::string fileName) {
    auto search = mMeshMap.find(fileName);
    if (search != mMeshMap.end()) {
        return &search->second; // Return a copy of the mesh it wants if already stored
    }
    if (!readTriangleFile(fileName)) { // Should run readTriangleFile and add the mesh to the Meshes map if it can be found
        qDebug() << "ResourceManager: Failed to find " << QString::fromStdString(fileName);
        return new MeshComponent;
    }
    mMeshMap[fileName] = *meshPool()->getLast(); // Save the new unique mesh in the meshmap for other entities that might need it
    return meshPool()->getLast();                // the mesh at the back is the latest creation
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

        MeshComponent *mesh = meshPool()->getLast();
        mesh->mVerticeCount = mMeshData.mVertices.size();
        mesh->mDrawType = GL_TRIANGLES;
        initVertexBuffers(mesh);
        return true;
    } else {
        qDebug() << "Could not open file for reading: " << QString::fromStdString(fileName);
        return false;
    }
}
//=========================== GETTERS ===========================//
Pool<MeshComponent> *ResourceManager::meshPool() {
    return mRenderView->getMeshPool();
}
Pool<MaterialComponent> *ResourceManager::matPool() {
    return mRenderView->getMaterialPool();
}
Pool<LightComponent> *ResourceManager::lightPool() {
    return mLightSystem->lightPool();
}
std::shared_ptr<Pool<TransformComponent>> ResourceManager::transformPool() const {
    return mTransforms;
}
void ResourceManager::setLightSystem(const std::shared_ptr<LightSystem> &lightSystem) {
    mLightSystem = lightSystem;
}

/**
 * @brief Get a raw pointer to mRenderView -- ResourceManager owns this as a unique_ptr
 * @return
 */
RenderView *ResourceManager::getRenderView() const {
    return mRenderView.get();
}

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