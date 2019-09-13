#ifndef HIERARCHYVIEW_H
#define HIERARCHYVIEW_H
#include <QTreeView>

class HierarchyView : public QTreeView {
    Q_OBJECT
public:
    HierarchyView(QWidget *parent = nullptr);

    void dropEvent(QDropEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
signals:
    void dragSelection(const QString &text);
};

#endif // HIERARCHYVIEW_H
