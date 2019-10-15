#include "hierarchyview.h"
#include "entity.h"
#include "hierarchymodel.h"
#include "mainwindow.h"
#include "registry.h"
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
HierarchyView::HierarchyView(QWidget *parent) : QTreeView(parent) {
}
void HierarchyView::dragEnterEvent(QDragEnterEvent *event) {
    QTreeView::dragEnterEvent(event);
    // Get the item from index (must cast to HierarchyModel or QStandardItemModel to get itemFromIndex function
    QStandardItem *item = static_cast<HierarchyModel *>(model())->itemFromIndex(QTreeView::currentIndex());
    Entity *entity = Registry::instance()->getEntity(item->text()); // One further cast to get the Entity, an overloaded QStandardItem that also contains an entity ID.
    if (entity)
        emit dragSelection(entity->id()); // When you start dragging an item, make sure you save that item in MainWindow's selectedEntity gameobject.
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

void HierarchyView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        // make options list
    } else {
        QTreeView::mousePressEvent(event);
    }
}
