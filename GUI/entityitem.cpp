#include "entityitem.h"

EntityItem::EntityItem(const QString &text, GLuint entityID) : QStandardItem(text), eID(entityID) {
}
