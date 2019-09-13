#ifndef HIERARCHYMODEL_H
#define HIERARCHYMODEL_H
#include <QStandardItemModel>

class HierarchyModel : public QStandardItemModel {
    Q_OBJECT
public:
    HierarchyModel();
signals:
    void parentChanged(const QModelIndex &index);

protected:
    //    QStringList mimeTypes() const override;
    //    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    //    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
};

#endif // HIERARCHYMODEL_H
