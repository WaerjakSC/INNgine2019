#ifndef ISYSTEM_H
#define ISYSTEM_H

#include "deltaTime.h"
#include <QObject>
namespace cjk {
class ISystem {
public:
    ISystem() {}

    virtual void update(DeltaTime deltaTime = 0.016) = 0;
};
} // namespace cjk

#endif // ISYSTEM_H
