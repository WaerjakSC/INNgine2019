#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Components/meshcomponent.h"
#include "Shaders/shader.h"
#include "gameobject.h"
#include "texture.h"
#include <QOpenGLFunctions_4_1_Core>

#include "Components/comppch.h"
#include "Shaders/shader.h"
#include "pool.h"
#include "texture.h"
class RenderView;
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
    // Gets the mesh
    MeshComponent *GetMesh(std::string name);
    // De-allocate loaded resources from memory
    void Clear();
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

    int largestOffset(int eID);

    std::vector<Component *> getComponents() const;

    std::vector<int> getEntityStart() const;

    std::vector<std::map<int, int> *> getCompIndex() const;

    RenderView *getRenderView() const;

private:
    // Private constructor
    ResourceManager();
    GLuint mNumGameObjects{0};
    // std::map(key, object) for easy resource storage
    std::map<ShaderType, Shader *> Shaders;
    std::map<std::string, Texture *> Textures;
    std::map<std::string, unsigned int> mMeshMap;

    // Temp mVertices/mIndices container. Cleared before each use.
    meshData mMeshData;
    MainWindow *mMainWindow;
    // Component vectors
    /** @example
     *      auto search = mInputs.at(entityID);
     *      // if entity owns this component, search equals the offset location in mComponents
            // (offset is the number of components created for the entity before this one.
            // if mInput was the first component created for this entity, the offset is 0.)
            if (search != mInputs.end()) {
            int componentIndex = mEntityLocation[entityID] + search; // Actual component index is the index of the Entity's first component + the offset
            return &mComponents[componentIndex]; // Return a copy of the mesh it wants if already stored
            }
     */
    // Component Offset Indices
    // Offset is the number of components created for "entityID" before the searched component.
    // i.e. if mInput was the first component created for this entity, the offset is 0.)
    // <entityID, offset>
    std::map<int, int> mInputs; // mInputs[n] is the n'th Entity's InputComponent -- would imply there's a dense InputComponent array with size > mInputs.at(n).
                                // DOES NOT mean that all 'n' Entities have InputComponents.
                                // If mInputs[n] = m, that means the InputComponent can be found at index 'mEntityLocation + m' in the dense array.
                                // Entities without this component will have mInputs.at(n) return -1 (or some other garbage value).
                                // This results in each of these small vectors becoming a list of entities with this component in addition to their value as an index for
                                // the densely packed mComponents.
    std::map<int, int> mLights;
    std::map<int, int> mMaterials;
    std::map<int, int> mMeshes;
    std::map<int, int> mPhysics;
    std::map<int, int> mSounds;
    std::map<int, int> mTransforms;
    // Each Entity has a vector of size n = TotalPossibleComponents that points to the offset of each component in mComponents
    //Holds the index of the first component created for each entity.
    std::vector<int> mEntityStart;          // mEntityStart[n] is the location of the (n+1)th entity's first component
    std::vector<Component *> mComponents;   // Dense Set of Components initially sorted by entity
    std::vector<GameObject *> mGameObjects; // Save GameObjects as pointers to avoid clipping of derived classes
    std::vector<std::map<int, int> *> mCompIndex{&mTransforms, &mMaterials, &mMeshes, &mLights, &mInputs, &mPhysics, &mSounds};

    Pool<MeshComponent> mMeshComps;
    Pool<MaterialComponent> mMatComps;
    Pool<TransformComponent> mTransComps;
    RenderView *mRenderView;
    void sortComponents();

    // OpenGL init functions
    void initVertexBuffers();
    void initIndexBuffers();

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
    bool hasComponent(CType type, int eID);
    bool hasComponents(int eID);
};

#endif // RESOURCEMANAGER_H
