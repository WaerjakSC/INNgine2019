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
class QStandardItem;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void insertGameObjects(std::vector<int> entities);
signals:
    void made3DObject(GLuint eID);
    void doubleClick(GLuint eID);
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

    void onDoubleClickedEntity(const QModelIndex &index);

private:
    void init();
    Ui::MainWindow *ui;

    HierarchyModel *hierarchy;
    HierarchyView *hView;
    GameObject *selectedEntity{nullptr};
    QWidget *mRenderWindowContainer;
    RenderWindow *mRenderWindow;
    void forEach(QAbstractItemModel *model, QString parentName, QStandardItem *child, QModelIndex parent = QModelIndex());
    void createActions();
};

#endif // MAINWINDOW_H
