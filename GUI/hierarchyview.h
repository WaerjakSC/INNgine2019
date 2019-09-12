#ifndef HIERARCHYVIEW_H
#define HIERARCHYVIEW_H
#include <QTreeView>

class HierarchyView : public QTreeView {
public:
    HierarchyView(QWidget *parent = nullptr);

    void dropEvent(QDropEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
};

#endif // HIERARCHYVIEW_H
