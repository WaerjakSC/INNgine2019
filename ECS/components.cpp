#include "components.h"
#include "constants.h"
#include "mainwindow.h"
#include "registry.h"
#include "resourcemanager.h"
#include "soundsystem.h"
Component::~Component() {
}

/*
void Input::wheelEvent(QWheelEvent *event){
    QPoint numDegrees = event->angleDelta() / 8;
    RenderWindow r;
    //if RMB, change the speed of the camera
    if (RMB) {
        if (numDegrees.y() < 1)
        r.setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
        r.setCameraSpeed(-0.001f);
    }
    event->accept();
}
*/
Sound::Sound(std::string name, bool loop, float gain)
    : mLooping(loop), mName(name), mGain(gain) {
    alGetError();
    alGenBuffers(1, &mBuffer);
    Registry::instance()->getSystem<SoundSystem>()->checkError("alGenBuffers");
    alGenSources(1, &mSource);
    Registry::instance()->getSystem<SoundSystem>()->checkError("alGenSources");
    alSourcef(mSource, AL_PITCH, 1.0f);
    alSourcef(mSource, AL_GAIN, gain);
}
