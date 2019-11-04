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

    void setEntityData(const Entity &value);
    bool operator==(const Entity &other) {
        return eID == other.eID;
    }
    void destroy();
    void newGeneration(GLuint id, const QString &text);
    bool isDestroyed() const;
    bool isEmpty();

signals:
    void nameChanged(const Entity &self);

private:
    GLuint eID;
    QString mName;
    GLuint mGeneration{0};
    bool mIsDestroyed{false};
};
#endif // ENTITY_H
