#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H
#include "isystem.h"
#include "pool.h"
class InputSystem : public ISystem {
public:
    InputSystem();
    void update(float deltaTime = 0.016) override;

private:
};

#endif // INPUTSYSTEM_H
