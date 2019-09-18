#ifndef HIERARCHYVIEW_H
#define HIERARCHYVIEW_H
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
    void dragSelection(const QString &text);

private:
    MainWindow *mMainWindow{nullptr};
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // HIERARCHYVIEW_H
