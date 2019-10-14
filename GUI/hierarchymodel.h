#ifndef HIERARCHYMODEL_H
#define HIERARCHYMODEL_H
#include "gsl_math.h"
#include <QMimeData>
#include <QStandardItemModel>
class Entity;
class EMimeData;
class HierarchyModel : public QStandardItemModel {
    Q_OBJECT
public:
    HierarchyModel();
    Entity *itemFromEntityID(GLuint eID);
    void removeItem(Entity *removedItem);
    bool mime(const EMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    void decodeDataRecursive(QDataStream &stream, QStandardItem *item);
public slots:
    void removeEntity(GLuint eID);

signals:
    void parentChanged(const QModelIndex &index);

protected:
    //    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    //    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    //    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
};
class EMimeData : public QMimeData {
    Q_OBJECT
public:
    EMimeData() : QMimeData() {}
    EMimeData(const EMimeData &other) : QMimeData() { this->mEntityID = other.entityID(); }
    ~EMimeData() {}

    GLuint entityID() const { return mEntityID; }

    void setEntityID(const GLuint &entityID) { mEntityID = entityID; }

private:
    GLuint mEntityID;
};
Q_DECLARE_METATYPE(EMimeData)
#endif // HIERARCHYMODEL_H
