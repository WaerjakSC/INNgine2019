#ifndef ENTITYITEM_H
#define ENTITYITEM_H

#include "gsl_math.h"
#include <QStandardItem>
class EntityItem : public QStandardItem {
public:
    EntityItem(const QString &text, GLuint entityID);
    GLuint id() { return eID; }

private:
    GLuint eID;
};

#endif // ENTITYITEM_H
