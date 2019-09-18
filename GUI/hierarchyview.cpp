#include "hierarchyview.h"
#include "mainwindow.h"
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
HierarchyView::HierarchyView(QWidget *parent) : QTreeView(parent) {
}
void HierarchyView::dragEnterEvent(QDragEnterEvent *event) {
    QTreeView::dragEnterEvent(event);
    emit dragSelection(QTreeView::currentIndex().data().toString()); // When you start dragging an item, make sure you save that item in MainWindow's selectedEntity gameobject.
}
void HierarchyView::dropEvent(QDropEvent *event) {
    QTreeView::dropEvent(event);
}
void HierarchyView::setMainWindow(MainWindow *window) {
    mMainWindow = window;
}
void HierarchyView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F)
        mMainWindow->keyPressEvent(event);
}

void HierarchyView::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F)
        mMainWindow->keyReleaseEvent(event);
}
