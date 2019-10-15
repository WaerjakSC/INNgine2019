#ifndef COMPONENT_H
#define COMPONENT_H
#include "matrix4x4.h"
#include "shader.h"
#include "vertex.h"
#include <QKeyEvent>
class MainWindow;

// must undefine for Frustum.
#undef near
#undef far

typedef gsl::Vector3D vec3;
typedef gsl::Matrix3x3 mat3;
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
    gsl::Vector3D mAmbientColor{0.3f, 0.3f, 0.3f};

    GLfloat mLightStrength{0.7f};
    gsl::Vector3D mLightColor{0.3f, 0.3f, 0.3f};

    GLfloat mSpecularStrength{0.3f};
    GLint mSpecularExponent{4};
    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
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
        mMatrix.setToIdentity();
    }
    Transform(vec3 position, vec3 rotation = 0, vec3 scale = 1) : Transform() {
        mPosition = position;
        mRotation = rotation;
        mScale = scale;
    }
    virtual void update() {}

    bool mMatrixOutdated{true};

    gsl::Vector3D mPosition{0};
    gsl::Vector3D mRelativePosition{0};
    gsl::Vector3D mRotation{0};
    gsl::Vector3D mScale{1};
    gsl::Matrix4x4 mMatrix;

    std::vector<GLuint> mChildren;

    int parentID = -1;
};
/**
 * @brief The MaterialComponent class holds the shader, texture unit and objectcolor
 */
struct Material : public Component {
    Material(ShaderType type = Color, GLuint texture = 0, gsl::Vector3D color = 1) : mObjectColor(color), mTextureUnit(texture), mShader(type) {
        mType = CType::Material;
    }
    Material(std::string type, GLuint texture = 0, gsl::Vector3D color = 1) : mObjectColor(color), mTextureUnit(texture) {
        mType = CType::Material;
        if (type == "color")
            mShader = Color;
        else if (type == "texture")
            mShader = Tex;
        else if (type == "phong")
            mShader = Phong;
    }
    virtual void update() {}

    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
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
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)

    GLuint mVerticeCount{0};
    GLuint mIndiceCount{0};
    GLenum mDrawType{0};

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

enum ColType{
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
    Collision(ColType type, vec3 size) : colType(type) {}
    virtual void update(){}
    ColType colType;
    bool mTrigger{false};

    /**
      * @brief Axis Aligned Bounding Box
      */
    typedef struct AABB{
        vec3 origin;
        vec3 position;
        vec3 size;  // Half size

        inline AABB() : size(2,2,2){}
        inline AABB(const vec3& o, const vec3& s) : origin(o), size(s) {}
    } AABB;


    /**
      * @brief Oriented Bounding Box
      */
    typedef struct OBB{
        vec3 position;
        vec3 size;

        //<------ Trenger rotasjon her

        // default constructor: lager en OBB ved origo
        inline OBB() : size(2,2,2) {}
        // alternativ constructor: lager en OBB på gitt posisjon og størrelse (half extents)
        inline OBB(const vec3& p, const vec3& s) {}
        // alternativ constructor: lager en OBB på gitt posisjon og størrelse (half extents) OG rotasjon wiihuu
        //inline OBB(const vec3& p, const vec3& s, const ROTASJON!? ) : position(p), size(s), ROTASJON {}
    } OBB;

    /**
      * @brief Sphere struct
      */
    typedef struct Sphere{
        vec3 position;
        float radius;

        // default constructor
        inline Sphere(): radius(3.0f) {};
        // constructor with radius and position params
        inline Sphere(const vec3& pos, const float& r): position(pos), radius(r) {}
    } Sphere;

    /**
      * @brief Plane struct
      */
    typedef struct Plane{
        vec3 normal;
        float distance;

        inline Plane() : normal(1,0,0){}
        inline Plane(const vec3& n, float d) : normal(n), distance(d){}
    } Plane;
};
/**
  * @brief Frustum struct
  */
// have to define these outside Collision, so we can use them for frustum & frustum culling. (might be a better way to do this?)
typedef Collision::Plane plane;
typedef Collision::Sphere sphere;
typedef Collision::AABB aABB;
typedef Collision::OBB oBB;
typedef struct Frustum {
    union {
        struct {
            plane top;
            plane bottom;
            plane left;
            plane right;
            plane near;
            plane far;
        } planeType;
        plane planes[6];
    };
    inline Frustum() { }
    vec3 Intersection(plane p1, plane p2, plane p3);
    void GetCorners(const Frustum& f, vec3* outCorners);
    bool Intersects(const Frustum& f, const sphere& s);
    float Classify(const aABB& aabb, const plane& plane);
    float Classify(const oBB& obb, const plane& plane);
    bool Intersects(const Frustum& f, const aABB& aabb);
    bool Intersects(const Frustum& f, const oBB& obb);

} Frustum;
#endif // COMPONENT_H
