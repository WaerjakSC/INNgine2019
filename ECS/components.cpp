#include "components.h"
#include "constants.h"
#include "mainwindow.h"
#include "registry.h"
#include "resourcemanager.h"
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
    checkError("alGenBuffers");
    alGenSources(1, &mSource);
    checkError("alGenSources");
    alSourcef(mSource, AL_PITCH, 1.0f);
    alSourcef(mSource, AL_GAIN, gain);
}
bool Sound::checkError(std::string name) {
    QString qname = QString::fromStdString(name);
    switch (alGetError()) {
    case AL_NO_ERROR:
        break;
    case AL_INVALID_NAME:
        qDebug() << "OpenAL Error: " + qname + ": Invalid name!\n";
        return false;
    case AL_INVALID_ENUM:
        qDebug() << "OpenAL Error: " + qname + ": Invalid enum!\n";
        return false;
    case AL_INVALID_VALUE:
        qDebug() << "OpenAL Error: " + qname + ": Invalid value!\n";
        return false;
    case AL_INVALID_OPERATION:
        qDebug() << "OpenAL Error: " + qname + ": Invalid operation!\n";
        return false;
    case AL_OUT_OF_MEMORY:
        qDebug() << "OpenAL Error: " + qname + ": Out of memory!\n";
        return false;
    default:
        break;
    }
    return true;
}
