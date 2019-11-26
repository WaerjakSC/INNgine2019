#ifndef COMPONENTLIST_H
#define COMPONENTLIST_H
#include "collisionsystem.h"
#include "gltypes.h"
#include "registry.h"
#include "resourcemanager.h"
#include <QColor>
#include <QWidget>
class Transform;
class Material;
class Mesh;
class Entity;
class QLabel;
class QCheckBox;
class QHBoxLayout;
class CustomDoubleSpinBox;
class HierarchyModel;
class QComboBox;
class HierarchyView;
class QGroupBox;
class VerticalScrollArea;
class MainWindow;
class ComponentList : public QWidget {
    Q_OBJECT
public:
    ComponentList(VerticalScrollArea *inScrollArea);
    void setupComponentList();

public slots:
    void addTransformComponent();
    void addMaterialComponent();
    void addMeshComponent();
    void addSoundComponent();
    void addPhysicsComponent();
    void addLightComponent();
    void addBSplineComponent();
    void addGameCameraComponent();
    void addAIComponent();
    void addAABBCollider();
    void addOBBCollider();
    void addSphereCollider();
    void addPlaneCollider();
    void addCylinderCollider();

signals:
    void posX(GLfloat xIn);
    void posY(GLfloat yIn);
    void posZ(GLfloat zIn);
    void rotX(GLfloat xIn);
    void rotY(GLfloat yIn);
    void rotZ(GLfloat zIn);
    void scaleX(GLfloat xIn);
    void scaleY(GLfloat yIn);
    void scaleZ(GLfloat zIn);
    void originX(double xIn);
    void originY(double xIn);
    void originZ(double xIn);
    void AABBSizeX(double xIn);
    void AABBSizeY(double yIn);
    void AABBSizeZ(double zIn);
    void OBBPositionX(double xIn);
    void OBBPositionY(double yIn);
    void OBBPositionZ(double zIn);
    void OBBSizeX(double xIn);
    void OBBSizeY(double yIn);
    void OBBSizeZ(double zIn);
    void spherePositionX(double xIn);
    void spherePositionY(double yIn);
    void spherePositionZ(double zIn);
    void sphereRadius(double radius);
    void planeNormalX(double xIn);
    void planeNormalY(double yIn);
    void planeNormalZ(double zIn);
    void planeDistance(double radius);
    void cylinderPositionX(double xIn);
    void cylinderPositionY(double yIn);
    void cylinderPositionZ(double zIn);
    void cylinderRadius(double radius);
    void cylinderHeight(double height);
    void newShader(int entityID, std::string shader);
    void changeHealth(int health);

private slots:
    void updatePosSpinBoxes(int state);
    void updatePositionVals(GLuint eID, vec3 newPos, bool isGlobal);
    void updateRotationVals(GLuint eID, vec3 newRot);
    void updateScaleVals(GLuint eID, vec3 newScale);

    void setPositionX(double xIn);
    void setPositionY(double yIn);
    void setPositionZ(double zIn);
    void setRotationX(double xIn);
    void setRotationY(double yIn);
    void setRotationZ(double zIn);
    void setScaleX(double xIn);
    void setScaleY(double yIn);
    void setScaleZ(double zIn);

private:
    void setColor();
    void setNewMesh();
    void setNewTextureFile();

    void setupTransformSettings(const Transform &component);
    void setupMaterialSettings(const Material &component);
    void setupAISettings(const AIComponent &ai);
    void setupMeshSettings(const Mesh &mesh);
    void setupAABBSettings(const AABB &col);
    void setupOBBSettings(const OBB &col);
    void setupSphereColliderSettings(const Sphere &col);
    void setupCylinderColliderSettings(const Cylinder &col);
    void setupPlaneColliderSettings(const Plane &col);
    void setupGameCameraSettings(const GameCamera &cam);
    void setupBSplinePointSettings(const BSplinePoint &point);
    template <typename CompType>
    inline void addComponent() {
        GLuint selectedEntity{registry->getSelectedEntity()};
        if (selectedEntity != 0) {
            if (!registry->contains<CompType>(selectedEntity))
                registry->add<CompType>(selectedEntity);
            setupComponentList();
        }
    }
    template <typename ColliderType>
    inline void addCollider() {
        GLuint selectedEntity{registry->getSelectedEntity()};
        if (selectedEntity != 0) {
            if (!registry->contains<ColliderType>(selectedEntity)) {
                registry->add<ColliderType>(selectedEntity);
            }
            setupComponentList();
        }
    }
    Registry *registry;
    QColor rgb;
    QStyle *fusion;
    QLabel *colorLabel, *texFileLabel, *objFileLabel;
    QCheckBox *abs;
    CustomDoubleSpinBox *xVal, *yVal, *zVal;

    VerticalScrollArea *scrollArea;
    CustomDoubleSpinBox *makeDoubleSpinBox(const double &num, QHBoxLayout *layout, const std::optional<float> &minRange = std::nullopt, const std::optional<float> &maxRange = std::nullopt);
    std::tuple<CustomDoubleSpinBox *, CustomDoubleSpinBox *, CustomDoubleSpinBox *> makeVectorBox(const vec3 &vector, QHBoxLayout *layout = nullptr,
                                                                                                  const std::optional<float> &minRange = std::nullopt, const std::optional<float> &maxRange = std::nullopt);
    QComboBox *makeObjectTypeBox(QGroupBox *objectTypeBox, const Collision &col);
    friend class MainWindow;
};

#endif // COMPONENTLIST_H
