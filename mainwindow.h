#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QWidget;
class RenderWindow;
class GameObject;
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

private:
    void init();
    Ui::MainWindow *ui;

    GameObject *selectedEntity;
    QWidget *mRenderWindowContainer;
    RenderWindow *mRenderWindow;
};

#endif // MAINWINDOW_H
