#ifndef ENTITY_H
#define ENTITY_H

#include "components.h"
#include "matrix4x4.h"
#include <QStandardItem>

class Entity : public QStandardItem {
public:
    Entity(GLuint ID, const QString &text = "");
    Entity();
    virtual ~Entity();
    GLuint id() const { return eID; }
    QString name() { return text(); }
    void setName(const QString &name) { setText(name); }
    CType &types() { return mTypes; }
    CType getTypes() const { return mTypes; }
    void setEntityData(const Entity &value, int role = Qt::UserRole + 1);
    Entity *cloneEntity() const;
    bool operator==(const Entity &other) {
        return eID == other.eID;
    }

private:
    GLuint eID;

    CType mTypes{CType::None};
};

#endif // ENTITY_H
