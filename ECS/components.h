#ifndef COMPONENT_H
#define COMPONENT_H
#include "colorshader.h"
#include "core.h"
#include "matrix3x3.h"
#include "matrix4x4.h"
#include "shader.h"
#include "tiny_obj_loader.h"
#include "vertex.h"
#include <QJSEngine>
#include <QKeyEvent>
#ifdef _WIN32
#include <al.h>
#include <alc.h>
#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif
// must undefine for Frustum.
#undef near
#undef far
using namespace cjk;
typedef gsl::Matrix3x3 mat3;
typedef gsl::Matrix4x4 mat4;
struct meshData {
    meshData() = default;
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
    std::string mName;
    void Clear() {
        mVertices.clear();
        mIndices.clear();
        mName.clear();
    }
};
/**
 * @brief The Component class is the base class for all components.
 */
class Component {

public:
    Component() = default;
    virtual ~Component() {}
};
struct Transform : Component {
    Transform() {
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
    Transform(vec3 pos, vec3 rot = 0, vec3 newScale = 1) : Transform() {
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
struct Material : public Component {
    Material(Ref<Shader> shader = nullptr, GLuint texture = 0, vec3 color = 1, GLfloat specStr = 0.3f, GLint specExp = 4);

    GLfloat mSpecularStrength;
    GLint mSpecularExponent;
    vec3 mObjectColor;
    GLuint mTextureUnit; //the actual texture to put into the uniform
    Ref<Shader> mShader;
};

struct Mesh : public Component {
    Mesh() {
    }
    Mesh(GLenum drawType, std::string name, GLuint verticeCount = 0, GLuint indiceCount = 0)
        : mVerticeCount(verticeCount), mIndiceCount(indiceCount), mDrawType(drawType), mName(name) {
    }
    Mesh(GLenum drawType, meshData data) : Mesh(drawType, data.mName, data.mVertices.size(), data.mIndices.size()) {
    }
    //    Mesh(const Mesh &other) {
    //        *this = other;
    //    }

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)

    GLuint mVerticeCount{0};
    GLuint mIndiceCount{0};
    GLenum mDrawType{0};
    bool mRendered{true};

    std::string mName;

    //    Mesh &operator=(const Mesh &other) {
    //        mVAO = other.mVAO;
    //        mVBO = other.mVBO;
    //        mEAB = other.mEAB; //holds the indices (Element Array Buffer - EAB)
    //        mName = other.mName;

    //        mVerticeCount = other.mVerticeCount;
    //        mIndiceCount = other.mIndiceCount;
    //        mDrawType = other.mDrawType;
    //        return *this;
    //    }
    bool operator==(const Mesh &other) {
        return mName == other.mName; // name of the obj/txt file should be enough to verify if they're the same.
    }
};
struct Light : public Component {
    Light(GLfloat ambStr = 0.3f, vec3 ambColor = vec3(0.3f, 0.3f, 0.3f),
          GLfloat lightStr = 0.7f, vec3 lightColor = vec3(0.3f, 0.3f, 0.3f),
          vec3 color = vec3(1.f, 1.f, 1.f))
        : mAmbientStrength(ambStr), mAmbientColor(ambColor), mLightStrength(lightStr),
          mLightColor(lightColor), mObjectColor(color) {
    }

    GLfloat mAmbientStrength;
    vec3 mAmbientColor;

    GLfloat mLightStrength;
    vec3 mLightColor;
    vec3 mObjectColor;
};
struct Input : public Component {
    Input() {
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
};
struct Physics : public Component {
    Physics(float speed = 1.0f) : mSpeed(speed) {}

    float mSpeed;
    vec3 mVelocity;
};
struct Sound : public Component {
    Sound() {}
    Sound(std::string name, bool loop = false, float gain = 1.0);

    bool mLooping{false};
    bool mPlaying{false}; // Might want to change this, but atm it will just be playing by default immediately
    bool mPaused{false};
    bool mOutDated{true};

    std::string mName; ///< The name of the sound source.
    ALuint mSource;    ///< The sound source.
    ALuint mBuffer;    ///< The data buffer.
    float mGain;
    vec3 mPosition;    ///< Vector containing source position.
    vec3 mVelocity{0}; ///< Vector containing source velocity.
};

/**
 * @brief The Collision component class is the base class for our various collider types.
 * Don't use on its own.
 */
struct Collision : public Component {
public:
    Collision() {}
    //    Collision(vec3 size) : colType(type) {}

    bool mTrigger{false};
    Mesh colliderMesh;
    Transform transform;
    bool isStatic{true};
};

/**
  * @brief Axis Aligned Bounding Box
  */
struct AABB : public Collision {
    vec3 origin{0};
    vec3 size; // Half size

    AABB(bool stat = true);
    inline AABB(const vec3 &o, const vec3 &s, bool stat = true) : AABB(stat) {
        origin = o;
        size = s;
    }
};

/**
  * @brief Oriented Bounding Box
  */
struct OBB : public Collision {
    vec3 position{0};
    vec3 size;
    mat3 orientation;

    // default constructor: lager en OBB ved origo
    inline OBB() : size(vec3(2, 2, 2)) {}
    // alternativ constructor: lager en OBB på gitt posisjon og størrelse (half extents)
    inline OBB(const vec3 &p, const vec3 &s) : position(p), size(s) {}
    // alternativ constructor: lager en OBB på gitt posisjon og størrelse (half extents) OG rotasjon wiihuu
    inline OBB(const vec3 &p, const vec3 &s, const mat3 &o) : position(p), size(s), orientation(o) {}
};

/**
  * @brief Sphere struct
  */
struct Sphere : public Collision {
    vec3 position;
    float radius;

    // default constructor
    inline Sphere(bool stat = true) : radius(1.0f) {
        isStatic = stat;
    };
    // constructor with radius and position params
    inline Sphere(const vec3 &pos, const float &r, bool stat = true) : position(pos), radius(r) {
        isStatic = stat;
    }
};

/**
  * @brief Plane struct
  */
struct Plane : public Collision {
    vec3 normal;
    float distance;

    inline Plane(bool stat = true) : normal(1, 0, 0) {
        isStatic = stat;
    }
    inline Plane(const vec3 &n, const float &d, bool stat = true) : normal(n), distance(d) {
        isStatic = stat;
    }
};

struct Cylinder : public Collision {
    vec3 position;
    float radius;
    float height;
    // default constructor
    inline Cylinder(bool stat = true) : radius(3.0f) {
        isStatic = stat;
    };
    // constructor with radius and position params
    inline Cylinder(const vec3 &pos, const float &r, const float &h, bool stat = true) : position(pos), radius(r), height(h) {
        isStatic = stat;
    }
};
/**
 * @brief The BSplineCurve struct
 */
struct BSplinePoint : Component {
    BSplinePoint() {
        qDebug() << "added BSplinePoint to entity.";
    }
};

// TD enemies følger en path fra A til B
// de kan bevege seg, ta skade, dø
enum NPCstates { MOVE,
                 DEATH,
                 GOAL_REACHED };

enum NPCevents { ENDPOINT_ARRIVED,
                 ITEM_TAKEN,
                 DAMAGE_TAKEN };

// GNOMER
struct AIcomponent : public Component {
    AIcomponent(int health = 100, int dmg = 10) : hp(health), damage(dmg) {}
    int hp = 100;
    int damage;
};

// BASEN
struct PlayerComponent : public Component {
    PlayerComponent() {}
    int playerHP = 100;
};

#endif // COMPONENT_H
