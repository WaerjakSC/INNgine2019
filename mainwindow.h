#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "components.h"
#include "core.h"
#include "gltypes.h"
#include "vector3d.h"
#include <QMainWindow>
#include <QStringListModel>

using namespace cjk;
class QWidget;
class RenderWindow;
class Entity;
class QStandardItem;
class HierarchyModel;
class HierarchyView;
class VerticalScrollArea;
class QToolButton;
class ComponentList;
class Registry;
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
    void clearEditor();

signals:
    void made3DObject(GLuint eID);
    void goToLoc(GLuint eID);
    void selectedEntity(GLuint eID);
public slots:
    void parentChanged(GLuint eID);
    void mouseRayHit(int eID);
    void closeEngine();

private slots:
    void onEntityClicked(const QModelIndex &index);

    void onNameChanged(const QModelIndex &index);
    void onEntityAdded(GLuint entity);
    void onEntityRemoved(GLuint entity);

    void onParentChanged(const QModelIndex &index);

    void makeCube();

    void makeSphere();

    void makePlane();

    void makeEntity();

    void changeEntityName(const GLuint &entt);

private:
    void init();
    Ui::MainWindow *ui;

    HierarchyModel *hierarchy;
    HierarchyView *hView;
    VerticalScrollArea *scrollArea;
    QWidget *mRenderWindowContainer;
    RenderWindow *mRenderWindow;
    ComponentList *mComponentList;
    quint32 unnamedEntityCount{0};

    Registry *registry;

    bool mShowingMsg{false};

    void forEach(GLuint parentID, QStandardItem *child, QModelIndex parent = QModelIndex());
    void createActions();
    void playButtons();
    friend class ComponentList;
    friend class ComponentGroupBox;
    friend class HierarchyView;
    friend class ResourceManager;
    friend class RenderWindow;
};

#endif // MAINWINDOW_H
