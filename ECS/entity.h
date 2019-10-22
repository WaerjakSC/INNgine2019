#ifndef ENTITY_H
#define ENTITY_H

#include "components.h"
#include "matrix4x4.h"
#include <QObject>

class Entity : public QObject {
    Q_OBJECT
public:
    Entity(GLuint ID, const QString &text = "");
    virtual ~Entity();
    GLuint id() const { return eID; }
    QString name() const { return mName; }
    void setName(const QString &name);
    CType &types() { return mTypes; }
    CType getTypes() const { return mTypes; }
    void setEntityData(const Entity &value);
    bool operator==(const Entity &other) {
        return eID == other.eID;
    }
signals:
    void nameChanged(const Entity &self);

private:
    GLuint eID;
    QString mName;
    CType mTypes{CType::None};
};
#endif // ENTITY_H
