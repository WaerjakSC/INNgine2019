#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "components.h"
#include "gltypes.h"
#include "vector3d.h"
#include <QMainWindow>
#include <QStringListModel>

class QWidget;
class RenderWindow;
class Entity;
class QStandardItem;
class HierarchyModel;
class HierarchyView;
class VerticalScrollArea;
class QLabel;
class Transform;
class Material;
class QToolButton;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void insertEntities();
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    QString windowName = "OpenGL in Qt";
    QToolButton *play, *pause, *stop;

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
    void parentChanged(GLuint eID);
    void mouseRayHit(GLuint eID);
private slots:
    void onEntityClicked(const QModelIndex &index);

    void onNameChanged(const QModelIndex &index);
    void onEntityAdded(GLuint entity);

    void onParentChanged(const QModelIndex &index);

    void onEntityDragged(GLuint id);

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
    void makeEntity();

private:
    void init();
    Ui::MainWindow *ui;

    HierarchyModel *hierarchy;
    HierarchyView *hView;
    VerticalScrollArea *scrollArea;
    Entity *selectedEntity{nullptr};
    QWidget *mRenderWindowContainer;
    RenderWindow *mRenderWindow;
    QColor rgb;
    QLabel *colorLabel, *texFileLabel, *objFileLabel;
    quint32 unnamedEntityCount{0};

    void forEach(GLuint parentID, QStandardItem *child, QModelIndex parent = QModelIndex());
    void createActions();
    void setupComponentList();
    void setupTransformSettings(const Transform &component);
    void setupMaterialSettings(const Material &component);
    void setupMeshSettings(const Mesh &mesh);
    void playButtons();
};

#endif // MAINWINDOW_H
