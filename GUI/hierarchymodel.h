#ifndef HIERARCHYMODEL_H
#define HIERARCHYMODEL_H
#include "gsl_math.h"
#include <QStandardItemModel>
class QMimeData;


class HierarchyModel : public QStandardItemModel {
    Q_OBJECT
public:
    HierarchyModel();
    QStandardItem *itemFromEntityID(GLuint eID);
public slots:
    void removeEntity(GLuint eID);

signals:
    void parentChanged(const QModelIndex &index);

protected:
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
};


#endif // HIERARCHYMODEL_H
