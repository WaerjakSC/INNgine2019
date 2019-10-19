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
class QDoubleSpinBox;
class HierarchyModel;
class HierarchyView;
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
    void newShader(int entityID, ShaderType shader);

private slots:
    void updatePosSpinBoxes(int state);
    void updatePositionVals(GLuint eID, vec3 newPos, bool isGlobal);
    void updateRotationVals(GLuint eID, vec3 newRot);
    void updateScaleVals(GLuint eID, vec3 newScale);

    void setNewShader(const QString &text);

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
    void setupTransformSettings(const Transform &component);
    void setColor();
    void setNewMesh();
    void setNewTextureFile();
    void setupMaterialSettings(const Material &component);
    void setupMeshSettings(const Mesh &mesh);

    Registry *registry;
    QColor rgb;
    QLabel *colorLabel, *texFileLabel, *objFileLabel;
    QCheckBox *abs;
    QDoubleSpinBox *xVal, *yVal, *zVal;

    VerticalScrollArea *scrollArea;
    MainWindow *mMainWindow;
    friend class MainWindow;
};

#endif // COMPONENTLIST_H
