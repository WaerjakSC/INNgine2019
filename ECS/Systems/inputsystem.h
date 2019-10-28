#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H
#include "isystem.h"
#include "pool.h"
class Registry;
class ResourceManager;
class InputSystem : public ISystem {
public:
    InputSystem(Camera *currentCam);
    void update(float deltaTime = 0.016) override;
signals:
    void snapSignal();
    void rayHitEntity(GLuint entityID);

private:
    void handleGlobalInput(QKeyEvent *event);
    Registry *registry{};
    ResourceManager *factory;
    Camera *mCurrentCamera{nullptr};
};

#endif // INPUTSYSTEM_H
