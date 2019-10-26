#ifndef COMPONENT_H
#define COMPONENT_H
#include "matrix3x3.h"
#include "matrix4x4.h"
#include "shader.h"
#include "tiny_obj_loader.h"
#include "vertex.h"
#include <QKeyEvent>
class MainWindow;

// must undefine for Frustum.
#undef near
#undef far

typedef gsl::Vector3D vec3;
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
struct LightData {
    LightData() {}
    LightData(GLfloat ambientStrength, vec3 ambientColor, GLfloat lightStrength, vec3 lightColor,
              GLfloat specularStrength, GLint specularExponent,
              vec3 objectColor)
        : mAmbientStrength(ambientStrength), mAmbientColor(ambientColor),
          mLightStrength(lightStrength), mLightColor(lightColor),
          mSpecularStrength(specularStrength), mSpecularExponent(specularExponent),
          mObjectColor(objectColor) {}
    GLfloat mAmbientStrength{0.3f};
    vec3 mAmbientColor{0.3f, 0.3f, 0.3f};

    GLfloat mLightStrength{0.7f};
    vec3 mLightColor{0.3f, 0.3f, 0.3f};

    GLfloat mSpecularStrength{0.3f};
    GLint mSpecularExponent{4};
    vec3 mObjectColor{1.f, 1.f, 1.f};
};
enum class CType {
    None = 0,
    Transform = 1 << 0,
    Material = 1 << 1,
    Mesh = 1 << 2,
    Light = 1 << 3,
    Input = 1 << 4,
    Physics = 1 << 5,
    Sound = 1 << 6,
    Collision = 1 << 7
};

template <typename E>
struct enableBitmaskOperators {
    static constexpr bool enable = false;
};
template <>
struct enableBitmaskOperators<CType> {
    static constexpr bool enable = true;
};
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E>::type
operator|(E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E>::type
operator~(E type) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(static_cast<underlying>(type));
}
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E &>::type
operator|=(E &lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    lhs = static_cast<E>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    return lhs;
}
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E &>::type
operator&=(E &lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    lhs = static_cast<E>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    return lhs;
}
template <typename E>
typename std::enable_if<enableBitmaskOperators<E>::enable, E>::type
operator&(E lhs, E rhs) {
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}
/**
 * @brief The Component class is the base class for all components.
 */
class Component {

public:
    Component() = default;
    virtual ~Component();

    virtual void update() = 0;

    CType type() const {
        return mType;
    }

protected:
    CType mType;
};
struct Transform : Component {
    Transform() {
        mType = CType::Transform;
        modelMatrix.setToIdentity();
        translationMatrix.setToIdentity();
        rotationMatrix.setToIdentity();
        scaleMatrix.setToIdentity();
    }
    Transform(vec3 pos, vec3 rot = 0, vec3 newScale = 1) : Transform() {
        localPosition = pos;
        localRotation = rot;
        localScale = newScale;
    }
    virtual void update() {}

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
    Material(ShaderType type = Color, GLuint texture = 0, vec3 color = 1) : mObjectColor(color), mTextureUnit(texture), mShader(type) {
        mType = CType::Material;
    }

    virtual void update() {}

    vec3 mObjectColor{1.f, 1.f, 1.f};
    GLuint mTextureUnit{0}; //the actual texture to put into the uniform
    ShaderType mShader;
};

struct Mesh : public Component {
    Mesh() {
        mType = CType::Mesh;
    }
    Mesh(GLenum drawType, std::string name, GLuint verticeCount = 0, GLuint indiceCount = 0)
        : mVerticeCount(verticeCount), mIndiceCount(indiceCount), mDrawType(drawType), mName(name) {
        mType = CType::Mesh;
    }
    Mesh(GLenum drawType, meshData data) : Mesh(drawType, data.mName, data.mVertices.size(), data.mIndices.size()) {
    }
    virtual void update() {}

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0};      //holds the indices (Element Array Buffer - EAB)
    GLuint mAABB_VAO{0}; // holds the VAO for the Axis Aligned Bounding Box

    GLuint mVerticeCount{0};
    GLuint mIndiceCount{0};
    GLenum mDrawType{0};
    bool mRendered{true};

    std::string mName;

    Mesh &operator=(const Mesh &other) {
        mVAO = other.mVAO;
        mVBO = other.mVBO;
        mEAB = other.mEAB; //holds the indices (Element Array Buffer - EAB)
        mName = other.mName;

        mVerticeCount = other.mVerticeCount;
        mIndiceCount = other.mIndiceCount;
        mDrawType = other.mDrawType;
        return *this;
    }
    bool operator==(const Mesh &other) {
        return mName == other.mName; // name of the obj/txt file should be enough to verify if they're the same.
    }
};
struct Light : public Component {
    Light(LightData light = LightData()) : mLight(light) {
        mType = CType::Light;
    }
    virtual void update() {}

    LightData mLight;
};
struct Input : public Component {
    Input() {
        mType = CType::Input;
    }
    virtual void update() {}

    bool W{false};
    bool A{false};
    bool S{false};
    bool D{false};
    bool L{false};
    bool UP{false}; //Arrow keys
    bool DOWN{false};
    bool LEFT{false};
    bool RIGHT{false};
    bool Q{false};
    bool E{false};
    bool F{false};
    bool C{false};
    bool LSHIFT{false};
    bool CTRL{false};
    bool SPACE{false};
    bool LMB{false};
    bool RMB{false};
    bool MMB{false};
    float MWHEEL{0.f};
    int MOUSEX{0};
    int MOUSEY{0};

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void setMainWindow(MainWindow *mainWindow) { mMainWindow = mainWindow; }

    void handleInput();
    MainWindow *mMainWindow;
};
struct Physics : public Component {
public:
    Physics() {}
    virtual void update() {}
};
struct Sound : public Component {
public:
    Sound() {}
    virtual void update() {}

    bool mLooping{false};
    bool mPlay{false};
    bool mPlaying{false};
};

enum ColType {
    AABB,
    OBB,
    Sphere,
    Capsule
};
/**
 * @brief The Collision component class holds the collider types and bounds
 */
struct Collision : public Component {
public:
    Collision() {}
    Collision(ColType type, vec3 size) : colType(type) {}
    virtual void update() {}
    ColType colType;
    bool mTrigger{false};
    GLuint mVAO{0}; // holds the VAO for the collider

    /**
      * @brief Axis Aligned Bounding Box
      */
    typedef struct AABB {
        vec3 origin;
        vec3 size; // Half size

        inline AABB() : size(2, 2, 2) {}
        inline AABB(const vec3 &o, const vec3 &s) : origin(o), size(s) {}
    } AABB;

    /**
      * @brief Oriented Bounding Box
      */
    typedef struct OBB {
        vec3 position;
        vec3 size;
        mat3 orientation;

        // default constructor: lager en OBB ved origo
        inline OBB() : size(2, 2, 2) {}
        // alternativ constructor: lager en OBB på gitt posisjon og størrelse (half extents)
        inline OBB(const vec3 &p, const vec3 &s) {}
        // alternativ constructor: lager en OBB på gitt posisjon og størrelse (half extents) OG rotasjon wiihuu
        inline OBB(const vec3 &p, const vec3 &s, const mat3 &o) : position(p), size(s), orientation(o) {}
    } OBB;

    /**
      * @brief Sphere struct
      */
    typedef struct Sphere {
        vec3 position;
        float radius;

        // default constructor
        inline Sphere() : radius(3.0f){};
        // constructor with radius and position params
        inline Sphere(const vec3 &pos, const float &r) : position(pos), radius(r) {}
    } Sphere;

    /**
      * @brief Plane struct
      */
    typedef struct Plane {
        vec3 normal;
        float distance;

        inline Plane() : normal(1, 0, 0) {}
        inline Plane(const vec3 &n, float d) : normal(n), distance(d) {}
    } Plane;

    typedef struct Cylinder {
        vec3 position;
        float radius;
        float height;

    } Cylinder;
};
/**
 * @brief The BSplineCurve struct
 */
struct BSplineCurve : Component {
    std::vector<vec3> b;  // control points
    int n;                // n number of knots
    int d;                // d degrees
    std::vector<float> t; // knots

    virtual void update() {}

    // default constructor
    BSplineCurve() {}
    BSplineCurve(std::vector<float> knots, std::vector<vec3> controlpoints, int degree = 2) : b(controlpoints), d(degree), t(knots) {
        n = knots.size();
    }
};

#endif // COMPONENT_H
