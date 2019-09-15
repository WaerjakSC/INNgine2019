#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    QAction *makeCube;
signals:
    void made3DObject();
public slots:
    void onGameObjectsChanged();
private slots:
    void onGameObjectClicked(const QModelIndex &index);

    void onNameChanged(const QModelIndex &index);

    void onParentChanged(const QModelIndex &index);

    void onGameObjectDragged(const QString &text);

    void make3DCube();

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
