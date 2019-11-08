#ifndef COMPONENTLIST_H
#define COMPONENTLIST_H
#include "gltypes.h"
#include "resourcemanager.h"
#include <QColor>
#include <QWidget>
class Transform;
class Material;
class Mesh;
class Registry;
class Entity;
class QLabel;
class QCheckBox;
class QHBoxLayout;
class QDoubleSpinBox;
class HierarchyModel;
class QComboBox;
class HierarchyView;
class QGroupBox;
class VerticalScrollArea;
class MainWindow;
class ComponentList : public QWidget {
    Q_OBJECT
public:
    ComponentList(MainWindow *window, VerticalScrollArea *inScrollArea);
    void setupComponentList();

public slots:
    void addSoundComponent();
    void addPhysicsComponent();
    void addInputComponent();
    void addLightComponent();
    void addMeshComponent();
    void addMaterialComponent();
    void addTransformComponent();
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

    void setNewShader(const QString &text);
    void setHealth(int health);

    void setPositionX(double xIn);
    void setPositionY(double yIn);
    void setPositionZ(double zIn);
    void setRotationX(double xIn);
    void setRotationY(double yIn);
    void setRotationZ(double zIn);
    void setScaleX(double xIn);
    void setScaleY(double yIn);
    void setScaleZ(double zIn);

    void setOriginX(double xIn);
    void setOriginY(double xIn);
    void setOriginZ(double xIn);
    void setAABBSizeX(double xIn);
    void setAABBSizeY(double yIn);
    void setAABBSizeZ(double zIn);
    void setOBBPositionX(double xIn);
    void setOBBPositionY(double yIn);
    void setOBBPositionZ(double zIn);
    void setOBBSizeX(double xIn);
    void setOBBSizeY(double yIn);
    void setOBBSizeZ(double zIn);
    void setSpherePositionX(double xIn);
    void setSpherePositionY(double yIn);
    void setSpherePositionZ(double zIn);
    void setSphereRadius(double radius);
    void setPlaneNormalX(double xIn);
    void setPlaneNormalY(double yIn);
    void setPlaneNormalZ(double zIn);
    void setPlaneDistance(double radius);
    void setCylinderPositionX(double xIn);
    void setCylinderPositionY(double yIn);
    void setCylinderPositionZ(double zIn);
    void setCylinderRadius(double radius);
    void setCylinderHeight(double height);

    void setObjectType(int index);

private:
    void setColor();
    void setNewMesh();
    void setNewTextureFile();

    void setupTransformSettings(const Transform &component);
    void setupMaterialSettings(const Material &component);
    void setupAISettings(const AIcomponent &ai);
    void setupMeshSettings(const Mesh &mesh);
    void setupAABBSettings(const AABB &col);
    void setupOBBSettings(const OBB &col);
    void setupSphereColliderSettings(const Sphere &col);
    void setupCylinderColliderSettings(const Cylinder &col);
    void setupPlaneColliderSettings(const Plane &col);
    Registry *registry;
    QColor rgb;
    QStyle *fusion;
    QLabel *colorLabel, *texFileLabel, *objFileLabel;
    QCheckBox *abs;
    QDoubleSpinBox *xVal, *yVal, *zVal;

    VerticalScrollArea *scrollArea;
    MainWindow *mMainWindow;
    friend class MainWindow;

    std::tuple<QDoubleSpinBox *, QDoubleSpinBox *, QDoubleSpinBox *> makeVectorBox(const vec3 &vector, QHBoxLayout *layout = nullptr,
                                                                                   const std::optional<float> &minRange = std::nullopt, const std::optional<float> &maxRange = std::nullopt);
    void updateAABB(GLuint eID);
    void updateSphere(GLuint eID);
    QComboBox *makeObjectTypeBox(QGroupBox *objectTypeBox, const Collision &col);
};

#endif // COMPONENTLIST_H
