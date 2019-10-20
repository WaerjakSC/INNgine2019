#include "inputsystem.h"
#include "registry.h"
InputSystem::InputSystem() {
    mInputs = Registry::instance()->registerComponent<Input>();
}

void InputSystem::update(float deltaTime) {
}
