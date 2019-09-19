#ifndef SOUNDCOMPONENT_H
#define SOUNDCOMPONENT_H

#include "component.h"

class SoundComponent : public Component {
public:
    SoundComponent();
    virtual void update(float dt);
};

#endif // SOUNDCOMPONENT_H
