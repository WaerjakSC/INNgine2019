#ifndef COMPONENT_H
#define COMPONENT_H
#include "core.h"
#include "gltypes.h"
#include "matrix4x4.h"
#include "sparseset.h"
#include "vertex.h"
#include <QColor>
#include <queue>

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

struct meshData {
    meshData() = default;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::string name;
    void Clear()
    {
        vertices.clear();
        indices.clear();
        name.clear();
    }
};
/**
 * @brief The Component class is the base class for all components.
 */
class Component {
public:
    using vec3 = gsl::Vector3D;

    Component() = default;
    virtual ~Component() {}
};
struct EInfo : Component {
    EInfo() = default;
    EInfo(QString nameIn) : name(nameIn) {}
    QString name;
    GLuint generation{0};
    bool isDestroyed{false};
};
struct BillBoard : Component {
    BillBoard(bool constantYUp = true, bool normalVersion = false) : constantYUp(constantYUp), normalVersion(normalVersion) {}

    //    gsl::Vector3D getNormal(gsl::Matrix4x4 mMatrix);
    //    gsl::Vector3D normal{0.f, 0.f, -1.f};

    bool constantYUp{true};
    bool normalVersion{false}; //flip between two ways to calculate forward direction
};
struct Transform : Component {

    Transform()
    {
        modelMatrix.setToIdentity();
        //calculate matrix from position, scale, rotation
        translationMatrix.setToIdentity();
        translationMatrix.translate(localPosition);

        rotationMatrix.setToIdentity();
        rotationMatrix.rotateX(localRotation.x);
        rotationMatrix.rotateY(localRotation.y);
        rotationMatrix.rotateZ(localRotation.z);

        scaleMatrix.setToIdentity();
        scaleMatrix.scale(localScale);
    }
    Transform(vec3 pos, vec3 rot = 0, vec3 newScale = 1) : Transform()
    {
        localPosition = pos;
        localRotation = rot;
        localScale = newScale;
    }
    bool matrixOutdated{true};

    vec3 localPosition{0};
    vec3 position{0};
    vec3 localRotation{0};
    vec3 rotation{0};
    vec3 localScale{1};
    //    vec3 scale{1};
    gsl::Matrix4x4 modelMatrix, translationMatrix, rotationMatrix, scaleMatrix;

    std::vector<GLuint> children;

    int parentID = -1;
};
/**
 * @brief The MaterialComponent class holds the shader, texture unit and objectcolor
 */
class Shader;
struct Material : public Component {
    Material(cjk::Ref<Shader> shaderIn = nullptr, GLuint texUnit = 0, vec3 color = vec3{1}, GLfloat specStr = 0.3f, GLint specExp = 4);

    GLfloat specularStrength;
    GLint specularExponent;
    vec3 objectColor;
    GLuint textureUnit; //the actual texture to put into the uniform
    cjk::Ref<Shader> shader;
};

struct Mesh : public Component {
    Mesh()
    {
    }
    Mesh(GLenum drawTypeIn, std::string meshName, GLuint numVertices = 0, GLuint numIndices = 0)
        : verticeCount(numVertices), indiceCount(numIndices), drawType(drawTypeIn), name(meshName)
    {
    }
    Mesh(GLenum drawTypeIn, meshData data) : Mesh(drawTypeIn, data.name, data.vertices.size(), data.indices.size())
    {
    }

    GLuint VAO{0};
    GLuint VBO{0};
    GLuint EAB{0}; //holds the indices (Element Array Buffer - EAB)

    GLuint verticeCount{0};
    GLuint indiceCount{0};
    GLenum drawType{0};
    bool rendered{true};

    std::string name;

    bool operator==(const Mesh &other)
    {
        return name == other.name; // name of the obj/txt file should be enough to verify if they're the same.
    }
};
struct Particle {
    gsl::Vector3D position, velocity;
    float life{-1.f}, size; // Remaining life of the particle. if < 0 : dead and unused.
    GLubyte r, b, g, a;
};
struct ParticleEmitter : public Component {
    ParticleEmitter(bool active = false, bool decay = false, size_t nrParticles = 100, int pps = 50,
                    const vec3 &initDir = vec3{0}, const QColor &initColor = QColor{255, 0, 0, 127},
                    float inSpeed = 0.5f, float inSize = 0.1f, float inSpread = 1.5f, float inLifeSpan = 5.f,
                    GLuint texUnit = 0)
        : isActive(active), shouldDecay(decay), numParticles(nrParticles), particlesPerSecond(pps), initialDirection(initDir), initialColor(initColor),
          speed(inSpeed), size(inSize), spread(inSpread), lifeSpan(inLifeSpan), textureUnit(texUnit),
          positionData(std::vector<GLfloat>(numParticles * 4)), colorData(std::vector<GLubyte>(numParticles * 4)),
          particles(std::vector<Particle>(numParticles))
    {
        initLifeSpan = lifeSpan;
    }
    void setNumParticles(size_t num);
    bool isActive;
    bool shouldDecay;

    size_t numParticles;
    int particlesPerSecond;

    vec3 initialDirection;
    QColor initialColor;

    float speed, size, spread, lifeSpan, initLifeSpan;

    GLuint textureUnit{0};
    GLuint VAO{0};
    GLuint EAB{0};
    GLuint quadVBO{0};
    GLuint positionBuffer{0};
    GLuint colorBuffer{0};
    std::vector<GLfloat> positionData;
    std::vector<GLubyte> colorData;
    std::vector<Particle> particles;

    bool initialized{false};

    int activeParticles{0};
    int lastUsedParticle{0};
};
struct Light : public Component {
    Light(GLfloat ambStr = 0.3f, vec3 ambColor = vec3{0.3f, 0.3f, 0.3f},
          GLfloat lightStr = 0.7f, vec3 lColor = vec3{0.3f, 0.3f, 0.3f},
          vec3 objColor = vec3{1.f, 1.f, 1.f})
        : ambientStrength(ambStr), ambientColor(ambColor), lightStrength(lightStr),
          lightColor(lColor), objectColor(objColor)
    {
    }

    GLfloat ambientStrength;
    vec3 ambientColor;

    GLfloat lightStrength;
    vec3 lightColor;
    vec3 objectColor;
};
struct Input : public Component {
    Input()
    {
    }

    bool W{false};
    bool A{false};
    bool S{false};
    bool D{false};
    bool Q{false};
    bool E{false};

    //Arrow keys
    bool UP{false};
    bool DOWN{false};
    bool LEFT{false};
    bool RIGHT{false};

    bool F{false};
    bool H{false};
    bool C{false};
    bool L{false};
    bool ESCAPE{false};
    bool LSHIFT{false};
    bool CTRL{false};
    bool SPACE{false};
    bool LMB{false};
    bool RMB{false};
    bool MMB{false};
    float MWHEEL{0.f};
    int MOUSEX{0};
    int MOUSEY{0};
    bool F1{false};
};

struct Sound : public Component {
    Sound() {}
    /**
    * @brief Sound constructor
    * @param name Name of the file to be loaded
    * @param loop Whether to loop or play only once
    * @param gain Not sure
    */
    Sound(std::string fileName, bool loop = false, float gainIn = 1.0) : looping(loop), name(fileName), gain(gainIn) {}
    bool looping{false};
    bool playing{false}; // Might want to change this, but atm it will just be playing by default immediately
    bool paused{false};
    bool outDated{true};
    bool initialized{false};

    std::string name; ///< The name of the sound source.
    ALuint source;    ///< The sound source.
    ALuint buffer;    ///< The data buffer.
    float gain;
    vec3 position;    ///< Vector containing source position.
    vec3 velocity{0}; ///< Vector containing source velocity.
};

/**
 * @brief The Collision component class is the base class for our various collider types.
 * Don't use on its own.
 */
struct Collision : public Component {
public:
    Collision() {}
    //    Collision(vec3 size) : colType(type) {}

    bool trigger{false};
    Mesh colliderMesh;
    Transform transform;
    bool isStatic{true};
    bool overlapEvent{false};

    SparseSet overlappedEntities;
};

/**
  * @brief Axis Aligned Bounding Box
  */
struct AABB : public Collision {
    vec3 origin{0};
    vec3 size; // Half size

    AABB(bool stat = true);
    inline AABB(const vec3 &o, const vec3 &s, bool stat = true) : AABB(stat)
    {
        origin = o;
        size = s;
    }
};

/**
  * @brief Sphere struct
  */
struct Sphere : public Collision {
    vec3 position;
    float radius;

    // default constructor
    inline Sphere(bool stat = true) : radius(1.0f)
    {
        isStatic = stat;
    };
    // constructor with radius and position params
    inline Sphere(const vec3 &pos, const float &r, bool stat = true) : position(pos), radius(r)
    {
        isStatic = stat;
    }
};

struct GameCamera : public Component {
    inline GameCamera(vec3 pos = vec3{0}, float pitch = 0, float yaw = 0, bool active = false) : mCameraPosition(pos), mPitch(pitch), mYaw(yaw), mIsActive(active)
    {
    }
    vec3 mCameraPosition;
    float mPitch, mYaw;
    bool mOutDated{true};
    bool mIsActive;
};

/**
 * @brief The BSplineCurve struct
 */
struct BSplinePoint : Component {
    BSplinePoint(vec3 loc = vec3{0}) : location(loc)
    {
    }
    vec3 location{0};
};

/**
 * @brief The Buildable struct is merely a tag to let the engine know whether to let the player place an object onto it or not.
 * Defaults to false in construction, should be set to true for objects that can be built on.
 */
struct Buildable : public Component {
    Buildable(bool canBuild = false) : isBuildable(canBuild)
    {
    }
    bool isBuildable;
};
// TD enemies følger en path fra A til B
// de kan bevege seg, ta skade, dø
enum class NPCstates { MOVE,
                       DEATH,
                       GOAL_REACHED };

enum class NPCevents { ENDPOINT_ARRIVED,
                       ITEM_TAKEN,
                       DAMAGE_TAKEN };

enum class TowerStates { IDLE,
                         ATTACK,
                         PLACEMENT };

enum class AttackType { SPLASH,
                        PHYSICAL,
                        MAGIC };
// GNOMES
struct AIComponent : public Component {
    AIComponent(int hp = 100, float speed = 0.1f) : health(hp), moveSpeed(speed) {}
    int health;
    float moveSpeed;
    float pathT{0};
    NPCstates state{NPCstates::MOVE};
    std::queue<NPCevents> notification_queue;
};
// Towers
struct TowerComponent : public Component {
    TowerComponent(int dmg = 10, float cd = 2.5f, float r = 13.f) : damage(dmg), range(r), cooldown(cd) {}
    int damage;
    float range;
    float projectileSpeed = 5.f;
    float cooldown;
    float curCooldown = 1.5f;
    GLuint targetID;
    TowerStates state{TowerStates::PLACEMENT};
    AttackType attackState = AttackType::PHYSICAL;
};
// BASE
struct PlayerComponent : public Component {
    PlayerComponent() {}
    int health = 100;
    int gold = 500;
    int kills = 0;
};

struct Bullet : public Component {
    Bullet() {}
    Bullet(vec3 route, int d, float s) : direction(route), damage(d), speed(s) {}
    vec3 direction;
    int damage;
    float speed;
};

#endif // COMPONENT_H
