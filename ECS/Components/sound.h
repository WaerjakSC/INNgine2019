#ifndef SOUNDCOMPONENT_H
#define SOUNDCOMPONENT_H

#include "component.h"

class Sound : public Component {
public:
    Sound();
    virtual void update(float dt);
};

#endif // SOUNDCOMPONENT_H
