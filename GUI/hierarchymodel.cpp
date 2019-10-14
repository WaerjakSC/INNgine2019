#include "hierarchymodel.h"
#include "registry.h"
#include <QBitArray>
#include <QDataStream>
#include <QDebug>
#include <QMimeData>
#include <QStack>
HierarchyModel::HierarchyModel() {
}

bool HierarchyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    const EMimeData *myData = qobject_cast<const EMimeData *>(data); // EMimeData holds an additional id value.
    bool success = mime(myData, action, row, column, parent);
    //    Registry *reg = Registry::instance();
    //    Entity *entity = reg->getEntity(myData->entityID());
    //    if (row == -1 && column == -1) {
    //        Entity *item = static_cast<Entity *>(itemFromIndex(parent)); // Check if the item was parented in the drop
    //        if (item) {
    //            //            Entity *child = static_cast<Entity *>(item->child(item->rowCount() - 1));
    //            //            child->setEntityData(*entity);
    //            qDebug() << entity->name() << entity->id();
    //        } else {
    //            static_cast<Entity *>(itemFromIndex(index(rowCount() - 1, 0)))->setEntityData(*entity);
    //        }
    //    } else { // If no parent and row/column aren't -1, the item was dropped at the bottom of the list.
    //        static_cast<Entity *>(itemFromIndex(index(row, column)))->setEntityData(*entity);
    //    }
    //    if (!Registry::instance()->contains(myData->entityID(), CType::Transform))
    //        return false;
    //    for (int row = 0; row < entity->rowCount(); row++) {
    //        for (auto child : reg->getChildren(entity->id()))
    //            if (entity->child(row) == reg->getEntity(child)) {
    //                static_cast<Entity *>(entity->child(row))->setEntityData(*reg->getEntity(child));
    //                break;
    //            }
    //    }
    //    emit parentChanged(parent);
    return success;
}
bool HierarchyModel::mime(const EMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    // check if the action is supported
    if (!data || !(action == Qt::CopyAction || action == Qt::MoveAction))
        return false;
    // check if the format is supported
    const QString format = "application/x-qstandarditemmodeldatalist";
    if (!data->hasFormat(format))
        return QAbstractItemModel::dropMimeData(data, action, row, column, parent);
    if (row > rowCount(parent))
        row = rowCount(parent);
    if (row == -1)
        row = rowCount(parent);
    if (column == -1)
        column = 0;
    // decode and insert
    QByteArray encoded = data->data(format);
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    //code based on QAbstractItemModel::decodeData
    // adapted to work with QStandardItem
    int top = INT_MAX;
    int left = INT_MAX;
    int bottom = 0;
    int right = 0;
    QVector<int> rows, columns;
    QVector<Entity *> items;
    while (!stream.atEnd()) {
        int r, c;
        Entity *item = Registry::instance()->getEntity(data->entityID())->cloneEntity();
        stream >> r >> c;
        decodeDataRecursive(stream, item);
        rows.append(r);
        columns.append(c);
        items.append(item);
        top = qMin(r, top);
        left = qMin(c, left);
        bottom = qMax(r, bottom);
        right = qMax(c, right);
    }
    // insert the dragged items into the table, use a bit array to avoid overwriting items,
    // since items from different tables can have the same row and column
    int dragRowCount = 0;
    int dragColumnCount = right - left + 1;
    // Compute the number of continuous rows upon insertion and modify the rows to match
    QVector<int> rowsToInsert(bottom + 1);
    for (int i = 0; i < rows.count(); ++i)
        rowsToInsert[rows.at(i)] = 1;
    for (int i = 0; i < rowsToInsert.count(); ++i) {
        if (rowsToInsert.at(i) == 1) {
            rowsToInsert[i] = dragRowCount;
            ++dragRowCount;
        }
    }
    for (int i = 0; i < rows.count(); ++i)
        rows[i] = top + rowsToInsert.at(rows.at(i));
    QBitArray isWrittenTo(dragRowCount * dragColumnCount);
    // make space in the table for the dropped data
    int colCount = columnCount(parent);
    if (colCount < dragColumnCount + column) {
        insertColumns(colCount, dragColumnCount + column - colCount, parent);
        colCount = columnCount(parent);
    }
    insertRows(row, dragRowCount, parent);
    row = qMax(0, row);
    column = qMax(0, column);
    Entity *parentItem = static_cast<Entity *>(itemFromIndex(parent));
    if (!parentItem)
        parentItem = static_cast<Entity *>(invisibleRootItem());
    QVector<QPersistentModelIndex> newIndexes(items.size());
    // set the data in the table
    for (int j = 0; j < items.size(); ++j) {
        int relativeRow = rows.at(j) - top;
        int relativeColumn = columns.at(j) - left;
        int destinationRow = relativeRow + row;
        int destinationColumn = relativeColumn + column;
        int flat = (relativeRow * dragColumnCount) + relativeColumn;
        // if the item was already written to, or we just can't fit it in the table, create a new row
        if (destinationColumn >= colCount || isWrittenTo.testBit(flat)) {
            destinationColumn = qBound(column, destinationColumn, colCount - 1);
            destinationRow = row + dragRowCount;
            insertRows(row + dragRowCount, 1, parent);
            flat = (dragRowCount * dragColumnCount) + relativeColumn;
            isWrittenTo.resize(++dragRowCount * dragColumnCount);
        }
        if (!isWrittenTo.testBit(flat)) {
            newIndexes[j] = index(destinationRow, destinationColumn, parentItem->index());
            isWrittenTo.setBit(flat);
        }
    }
    for (int k = 0; k < newIndexes.size(); k++) {
        if (newIndexes.at(k).isValid()) {
            parentItem->setChild(newIndexes.at(k).row(), newIndexes.at(k).column(), items.at(k));
        } else {
            delete items.at(k);
        }
    }
    return true;
}

void HierarchyModel::decodeDataRecursive(QDataStream &stream, QStandardItem *item) {
    int colCount, childCount;
    stream >> *item;
    stream >> colCount >> childCount;
    item->setColumnCount(colCount);
    int childPos = childCount;
    while (childPos > 0) {
        childPos--;
        Entity *child = new Entity;
        decodeDataRecursive(stream, child);
        item->setChild(childPos / colCount, childPos % colCount, child);
    }
}
QMimeData *HierarchyModel::mimeData(const QModelIndexList &indexes) const {
    if (indexes.count() <= 0)
        return 0;
    QStringList types = mimeTypes();
    if (types.isEmpty())
        return 0;
    EMimeData *data = new EMimeData();
    QString format = types.at(0);
    QByteArray encoded1;
    QDataStream stream1(&encoded1, QIODevice::WriteOnly);
    encodeData(indexes, stream1);
    data->setData(format, encoded1);
    if (!data)
        return 0;
    format = "application/x-qstandarditemmodeldatalist";
    if (!mimeTypes().contains(format))
        return data;
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    QSet<QStandardItem *> itemsSet;
    QStack<QStandardItem *> stack;
    itemsSet.reserve(indexes.count());
    stack.reserve(indexes.count());
    for (int i = 0; i < indexes.count(); ++i) {
        if (QStandardItem *item = itemFromIndex(indexes.at(i))) {
            itemsSet << item;
            stack.push(item);
        } else {
            qWarning("QStandardItemModel::mimeData: No item associated with invalid index");
            return 0;
        }
    }
    //stream everything recursively
    while (!stack.isEmpty()) {
        QStandardItem *item = stack.pop();
        if (itemsSet.contains(item)) //if the item is selection 'top-level', stream its position
            stream << item->row() << item->column();
        stream << *item << item->columnCount() << item->rowCount();
        for (int i = 0; i < item->rowCount(); i++)
            stack += item->child(i);
    }
    Entity *item = static_cast<Entity *>(itemFromIndex(indexes.at(0)));

    data->setEntityID(item->id());
    data->setData(format, encoded);

    return data;
}

/**
 * @brief Slot to remove an item from the tree view if it's removed for any reason. Untested but should update the parent/child relationships as it happens
 * @param eID
 */
void HierarchyModel::removeEntity(GLuint eID) {
    for (int i = 0; i < rowCount(); i++) {
        QModelIndex idx = this->index(i, 0);
        Entity *item = static_cast<Entity *>(itemFromIndex(idx));
        if (item->id() == eID) {
            emit parentChanged(idx.parent()); // Emit parentChanged with the removed Entity's parent as the new parent of all its children
            removeRow(i);
            return;
        }
    }
}

void HierarchyModel::removeItem(Entity *removedItem) {
    for (int i = 0; i < rowCount(); i++) {
        QModelIndex idx = this->index(i, 0);
        Entity *item = static_cast<Entity *>(itemFromIndex(idx));
        if (item->id() == removedItem->id()) {
            removeRow(i);
            return;
        }
    }
}

Entity *HierarchyModel::itemFromEntityID(GLuint eID) {
    for (int i = 0; i < rowCount(); i++) {
        QModelIndex idx = this->index(i, 0);
        Entity *item = static_cast<Entity *>(itemFromIndex(idx));
        if (item->id() == eID) {
            return item;
        }
    }
    return nullptr;
}
