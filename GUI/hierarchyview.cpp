#include "hierarchyview.h"
#include "entity.h"
#include "hierarchymodel.h"
#include "inputsystem.h"
#include "mainwindow.h"
#include "registry.h"
#include <QAction>
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
HierarchyView::HierarchyView(QWidget *parent) : QTreeView(parent) {
    setContextMenuPolicy(Qt::ActionsContextMenu);
    createContextActions();
}
void HierarchyView::dragEnterEvent(QDragEnterEvent *event) {
    QTreeView::dragEnterEvent(event);
    // Get the item from index (must cast to HierarchyModel or QStandardItemModel to get itemFromIndex function
    QStandardItem *item = static_cast<HierarchyModel *>(model())->itemFromIndex(QTreeView::currentIndex());
    Entity *entity = Registry::instance()->getEntity(item->data().toInt()).get(); // One further cast to get the Entity, an overloaded QStandardItem that also contains an entity ID.
    if (entity)
        emit dragSelection(entity->id()); // When you start dragging an item, make sure you save that item in MainWindow's selectedEntity gameobject.
}
void HierarchyView::createContextActions() {
    QAction *rename = new QAction(tr("Rename"), this);
    connect(rename, &QAction::triggered, this, &HierarchyView::renameEntity);
    addAction(rename);
    QAction *duplicate = new QAction(tr("Duplicate"), this);
    connect(duplicate, &QAction::triggered, this, &HierarchyView::duplicateEntity);
    addAction(duplicate);
    QAction *player = new QAction(tr("Make Player Controller"), this);
    connect(player, &QAction::triggered, this, &HierarchyView::setController);
    addAction(player);
    QAction *remove = new QAction(tr("Remove"), this);
    connect(remove, &QAction::triggered, this, &HierarchyView::removeEntity);
    addAction(remove);
}
void HierarchyView::setController() {
    auto inputsys = Registry::instance()->getSystem<InputSystem>();
    inputsys->setPlayer(rightClickEntity);
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
void HierarchyView::renameEntity() {
    edit(currentIndex());
}

void HierarchyView::removeEntity() {
    Registry::instance()->removeEntity(rightClickEntity);
}
void HierarchyView::duplicateEntity() {
    GLuint newEntity = Registry::instance()->duplicateEntity(rightClickEntity);
    HierarchyModel *hModel = static_cast<HierarchyModel *>(model());
    QModelIndex index = hModel->itemFromEntityID(newEntity)->index();
    setCurrentIndex(index);
}
void HierarchyView::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F)
        mMainWindow->keyReleaseEvent(event);
}
void HierarchyView::mousePressEvent(QMouseEvent *event) {
    QTreeView::mousePressEvent(event);
    if (event->button() == Qt::RightButton) {
        QStandardItem *item = static_cast<HierarchyModel *>(model())->itemFromIndex(QTreeView::currentIndex());
        rightClickEntity = item->data(257).toUInt();
    }
}
