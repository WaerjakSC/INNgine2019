#ifndef COMPONENT_H
#define COMPONENT_H
#include "matrix4x4.h"
#include "shader.h"
#include "vertex.h"
#include <QKeyEvent>
class MainWindow;

typedef gsl::Vector3D vec3;
struct meshData {
    meshData() = default;
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;
    void Clear() {
        mVertices.clear();
        mIndices.clear();
    }
};
struct LightData {
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
    Sound = 1 << 6
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

    std::vector<int> mChildren;
    bool hasChildren{false};

    int parentID = -1;
};
/**
 * @brief The MaterialComponent class holds the shader, texture unit and objectcolor
 */
struct Material : public Component {
    Material(ShaderType type = Color, GLuint texture = 0, gsl::Vector3D color = 1) : mObjectColor(color), mTextureUnit(texture), mShader(type) {
        mType = CType::Material;
    }
    virtual void update() {}

    gsl::Vector3D mObjectColor{1.f, 1.f, 1.f};
    GLuint mTextureUnit{0}; //the actual texture to put into the uniform
    ShaderType mShader;
};

struct Mesh : public Component {
public:
    Mesh() {}
    Mesh(GLenum drawType, GLuint verticeCount = 0, GLuint indiceCount = 0) : mVerticeCount(verticeCount), mIndiceCount(indiceCount), mDrawType(drawType) {
        mType = CType::Mesh;
    }
    Mesh(GLenum drawType, meshData data) : Mesh(drawType, data.mVertices.size(), data.mIndices.size()) {
    }

    virtual void update() {}

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; //holds the indices (Element Array Buffer - EAB)

    GLuint mVerticeCount{0};
    GLuint mIndiceCount{0};
    GLenum mDrawType{0};
    Mesh &operator=(const Mesh &other) {
        mVAO = other.mVAO;
        mVBO = other.mVBO;
        mEAB = other.mEAB; //holds the indices (Element Array Buffer - EAB)

        mVerticeCount = other.mVerticeCount;
        mIndiceCount = other.mIndiceCount;
        mDrawType = other.mDrawType;
        return *this;
    }
    bool operator==(const Mesh &other) {
        bool buffers = mVAO == other.mVAO && mVBO == other.mVBO && mEAB == other.mEAB;
        bool data = mVerticeCount == other.mVerticeCount && mIndiceCount == other.mIndiceCount && mDrawType == other.mDrawType;
        return buffers && data;
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
};
#endif // COMPONENT_H
