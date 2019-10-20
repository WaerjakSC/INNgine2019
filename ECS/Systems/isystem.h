#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <QObject>

class ISystem {
public:
    ISystem() {}

    virtual void update(float deltaTime = 0.016) = 0;
};

#endif // ISYSTEM_H
