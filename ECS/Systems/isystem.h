#ifndef ISYSTEM_H
#define ISYSTEM_H

#include "deltaTime.h"
#include <QObject>
/**
 * @brief The ISystem class is a simple interface class for systems.
 */
class ISystem {
public:
    ISystem() {}

    virtual void update(DeltaTime deltaTime = 0.016) = 0;
};

#endif // ISYSTEM_H
