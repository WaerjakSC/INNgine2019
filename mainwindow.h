#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "components.h"
#include "core.h"
#include "gltypes.h"
#include <QMainWindow>
#include <QModelIndex>
class QWidget;
class RenderWindow;
class QStandardItem;
class QToolButton;

class HierarchyModel;
class HierarchyView;
class VerticalScrollArea;
class ComponentGroupBox;
class ComponentList;
class Registry;
class ResourceManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
    * Initial insertion of entities, such as those made in an init function or read from a level file.
    */
    void insertEntities();
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    /**
     * Refreshes the list of entities in the scene.
     */
    void clearEditor();

    /**
     * Whether or not the window is showing a custom message.
     * @return
     */
    bool showingMsg() const;

    void setShowingMsg(bool showingMsg);

signals:
    void made3DObject(GLuint eID);
    void goToLoc(GLuint eID);
    void selectedEntity(GLuint eID);
    void renderStatus(Qt::CheckState state, GLuint entityID);

public slots:
    void parentChanged(GLuint eID);
    /**
     * Set the selected entity based on what was clicked on in the scene.
     * @param eID
     */
    void mouseRayHit(int eID);
    void closeEngine();

    void onMeshAdded(GLuint eID);

    void updateRenderedCheckBox(GLuint entityID, Qt::CheckState state);
private slots:
    void onEntityClicked(const QModelIndex &index);

    void onDataChanged(const QModelIndex &index, const QModelIndex &otherIndex = QModelIndex(), const QVector<int> roles = QVector<int>());
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
    ComponentList *mComponentList;
    QWidget *mRenderWindowContainer;
    RenderWindow *mRenderWindow;

    int IDRole{257};

    Registry *registry;

    bool mShowingMsg{false};

    void forEach(GLuint parentID, QStandardItem *child, QModelIndex parent = QModelIndex());
    void createActions();
    void playButtons();
};

#endif // MAINWINDOW_H
