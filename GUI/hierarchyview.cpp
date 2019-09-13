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
    //        qDebug() << QTreeView::currentIndex().data();
    emit QTreeView::currentIndex().data();
    //        qDebug() << event->mimeData()->text();
}
