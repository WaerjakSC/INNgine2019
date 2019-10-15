#include "hierarchymodel.h"
#include "registry.h"
#include <QDebug>
#include <QMimeData>

HierarchyModel::HierarchyModel() {
}

bool HierarchyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    bool success = QStandardItemModel::dropMimeData(data, action, row, column, parent);
    emit parentChanged(parent);
    return success;
}

/**
 * @brief Slot to remove an item from the tree view if it's removed for any reason. Untested but should update the parent/child relationships as it happens
 * @param eID
 */
void HierarchyModel::removeEntity(GLuint eID) {
    for (int i = 0; i < rowCount(); i++) {
        QModelIndex idx = this->index(i, 0);
        QStandardItem *item = itemFromIndex(idx);
        Entity *entt = Registry::instance()->getEntity(item->text());
        if (entt->id() == eID) {
            emit parentChanged(idx.parent()); // Emit parentChanged with the removed Entity's parent as the new parent of all its children
            removeRow(i);
            return;
        }
    }
}

QStandardItem *HierarchyModel::itemFromEntityID(GLuint eID) {
    for (int i = 0; i < rowCount(); i++) {
        QModelIndex idx = this->index(i, 0);
        QStandardItem *item = itemFromIndex(idx);
        Entity *entt = Registry::instance()->getEntity(item->text());
        if (entt->id() == eID) {
            return item;
        }
    }
    return nullptr;
}
