#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

class QWidget;
class RenderWindow;
class GameObject;
class HierarchyModel;
class HierarchyView;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void insertGameObjects(std::vector<GameObject *> entities);
public slots:
    void onGameObjectsChanged();
private slots:
    void on_pushButton_clicked();
    void onGameObjectClicked(const QModelIndex &index);

    void onNameChanged(const QModelIndex &index);

    void onParentChanged(const QModelIndex &index);

    void onGameObjectDragged(const QString &text);

private:
    void init();
    Ui::MainWindow *ui;

    HierarchyModel *hierarchy;
    HierarchyView *hView;
    GameObject *selectedEntity{nullptr};
    QWidget *mRenderWindowContainer;
    RenderWindow *mRenderWindow;
};

#endif // MAINWINDOW_H
