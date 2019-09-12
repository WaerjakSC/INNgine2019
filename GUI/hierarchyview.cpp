#include "hierarchyview.h"
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
HierarchyView::HierarchyView(QWidget *parent) : QTreeView(parent) {
}
void HierarchyView::dragEnterEvent(QDragEnterEvent *event) {
    QTreeView::dragEnterEvent(event);
}
void HierarchyView::dropEvent(QDropEvent *event) {
    QTreeView::dropEvent(event);
    //    qDebug() << event->mimeData()->text();
}
