#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "components.h"
#include "gltypes.h"
#include "vector3d.h"
#include <QMainWindow>
#include <QStringListModel>

class QWidget;
class RenderWindow;
class GameObject;
class HierarchyModel;
class HierarchyView;
class VerticalScrollArea;
class EntityItem;
class QLabel;
class Transform;
class Material;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void insertGameObjects();
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
signals:
    void made3DObject(GLuint eID);
    void goToLoc(GLuint eID);
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
public slots:
private slots:
    void onGameObjectClicked(const QModelIndex &index);

    void onNameChanged(const QModelIndex &index);
    void onGameObjectsChanged(GLuint entity);

    void onParentChanged(const QModelIndex &index);

    void onGameObjectDragged(GLuint id);

    void makeCube();

    void makeSphere();

    void makePlane();

    void snapToObject();

    void setPositionX(double xIn);
    void setPositionY(double yIn);
    void setPositionZ(double zIn);

    void setRotationX(double xIn);
    void setRotationY(double yIn);
    void setRotationZ(double zIn);

    void setScaleX(double xIn);
    void setScaleY(double yIn);
    void setScaleZ(double zIn);
    void updatePositionVals(GLuint eID, gsl::Vector3D newPos);
    void updateRotationVals(GLuint eID, gsl::Vector3D newRot);
    void updateScaleVals(GLuint eID, gsl::Vector3D newScale);

    void setNewShader(const QString &text);
    void setNewTextureFile();
    void setColor();
    void setNewMesh();

    void addTransformComponent();
    void addMaterialComponent();
    void addMeshComponent();
    void addLightComponent();
    void addInputComponent();
    void addPhysicsComponent();
    void addSoundComponent();
    void makeGameObject();

    void removeGameObject(const QModelIndex &index);

private:
    void init();
    Ui::MainWindow *ui;

    HierarchyModel *hierarchy;
    HierarchyView *hView;
    VerticalScrollArea *scrollArea;
    GameObject *selectedEntity{nullptr};
    QWidget *mRenderWindowContainer;
    RenderWindow *mRenderWindow;
    QColor rgb;
    QLabel *colorLabel;
    QLabel *texFileLabel;
    QLabel *objFileLabel;

    void forEach(QAbstractItemModel *model, QString parentName, EntityItem *child, QModelIndex parent = QModelIndex());
    void createActions();
    void setupComponentList();
    void setupTransformSettings(const Transform &component);
    void setupMaterialSettings(const Material &component);
    void setupMeshSettings(const Mesh &mesh);
};

#endif // MAINWINDOW_H
