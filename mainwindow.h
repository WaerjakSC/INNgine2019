#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gltypes.h"
#include <QMainWindow>
#include <QStringListModel>

class QWidget;
class RenderWindow;
class GameObject;
class HierarchyModel;
class HierarchyView;
class VerticalScrollArea;
class QStandardItem;
class TransformComponent;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void insertGameObjects(std::vector<int> entities);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
signals:
    void made3DObject(GLuint eID);
    void goToLoc(GLuint eID);
public slots:
private slots:
    void onGameObjectClicked(const QModelIndex &index);

    void onNameChanged(const QModelIndex &index);
    void onGameObjectsChanged(GLuint entity);

    void onParentChanged(const QModelIndex &index);

    void onGameObjectDragged(const QString &text);

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

private:
    void init();
    Ui::MainWindow *ui;

    HierarchyModel *hierarchy;
    HierarchyView *hView;
    VerticalScrollArea *scrollArea;
    GameObject *selectedEntity{nullptr};
    QWidget *mRenderWindowContainer;
    RenderWindow *mRenderWindow;
    void forEach(QAbstractItemModel *model, QString parentName, QStandardItem *child, QModelIndex parent = QModelIndex());
    void createActions();
    void setupComponentList();
    void setupTransformSettings(TransformComponent *component);
};

#endif // MAINWINDOW_H
