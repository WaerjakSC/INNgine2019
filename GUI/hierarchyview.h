#ifndef HIERARCHYVIEW_H
#define HIERARCHYVIEW_H
#include "gltypes.h"
#include <QTreeView>

class MainWindow;
class HierarchyView : public QTreeView {
    Q_OBJECT
public:
    HierarchyView(QWidget *parent = nullptr);

    void dropEvent(QDropEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void setMainWindow(MainWindow *window);
signals:
    void dragSelection(GLuint id);
    void selectedEntity(GLuint id);

private slots:
    void renameEntity();
    void removeEntity();
    void duplicateEntity();

private:
    MainWindow *mMainWindow{nullptr};
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void createContextActions();
    GLuint rightClickEntity;
    void setController();
};

#endif // HIERARCHYVIEW_H
