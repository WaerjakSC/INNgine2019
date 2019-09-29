#ifndef HIERARCHYVIEW_H
#define HIERARCHYVIEW_H
#include "gsl_math.h"
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

private:
    MainWindow *mMainWindow{nullptr};
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // HIERARCHYVIEW_H
