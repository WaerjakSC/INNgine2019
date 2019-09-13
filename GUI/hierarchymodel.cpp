#include "hierarchymodel.h"
#include <QDataStream>
#include <QDebug>
#include <QMimeData>
HierarchyModel::HierarchyModel() {
}

bool HierarchyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    bool success = QStandardItemModel::dropMimeData(data, action, row, column, parent);
    emit parentChanged(parent);
    return success;
}
