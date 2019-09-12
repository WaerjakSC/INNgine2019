#include "hierarchymodel.h"
#include <QDataStream>
#include <QDebug>
#include <QMimeData>
HierarchyModel::HierarchyModel() {
}

//QStringList HierarchyModel::mimeTypes() const {
//    QStringList types;
//    types << "application/vnd.text.list";
//    return types;
//}
//QMimeData *HierarchyModel::mimeData(const QModelIndexList &indexes) const {
//    QMimeData *mimeData = new QMimeData;
//    QByteArray encodedData;

//    QDataStream stream(&encodedData, QIODevice::WriteOnly);

//    for (const QModelIndex &index : indexes) {
//        if (index.isValid()) {
//            QString text = data(index, Qt::DisplayRole).toString();
//            stream << text;
//        }
//    }
//    mimeData->setData("application/vnd.text.list", encodedData);
//    return mimeData;
//}
//bool HierarchyModel::canDropMimeData(const QMimeData *data,
//                                     Qt::DropAction action, int row, int column, const QModelIndex &parent) const {
//    Q_UNUSED(action);
//    Q_UNUSED(row);
//    Q_UNUSED(parent);

//    if (!data->hasFormat("application/vnd.text.list"))
//        return false;

//    if (column > 0)
//        return false;

//    return true;
//}
//bool HierarchyModel::dropMimeData(const QMimeData *data,
//                                  Qt::DropAction action, int row, int column, const QModelIndex &parent) {
//    if (!canDropMimeData(data, action, row, column, parent))
//        return false;

//    if (action == Qt::IgnoreAction)
//        return true;
//    int beginRow;
//    int beginColumn;

//    if (row != -1)
//        beginRow = row;
//    else if (parent.isValid()) {
//        beginRow = parent.row();
//        beginColumn = parent.column() + 1;
//    } else
//        beginRow = rowCount(QModelIndex());
//    QByteArray encodedData = data->data("application/vnd.text.list");
//    QDataStream stream(&encodedData, QIODevice::ReadOnly);
//    QStringList newItems;
//    int rows = 0;

//    while (!stream.atEnd()) {
//        QString text;
//        stream >> text;
//        newItems << text;
//        ++rows;
//    }
//    qDebug() << newItems.last();
//    insertRows(beginRow, rows, QModelIndex());
//    insertColumns(beginColumn, 1, QModelIndex());
//    for (const QString &text : qAsConst(newItems)) {
//        QModelIndex idx = index(beginRow, beginColumn, QModelIndex());
//        setData(idx, text);
//        beginRow++;
//    }

//    return true;
//}
