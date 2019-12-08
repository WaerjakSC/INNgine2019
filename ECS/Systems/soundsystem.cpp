#include "soundsystem.h"
#include "cameracontroller.h"
#include "registry.h"
#include "resourcemanager.h"
namespace cjk {
SoundSystem::SoundSystem() : reg{Registry::instance()}
{
}

void SoundSystem::cleanUp()
{
    auto view{reg->view<Sound>()};
    for (auto entity : view) {
        Sound &sound{view.get(entity)};
        deleteSound(sound);
    }
    ResourceManager::instance()->clearContext();
}
void SoundSystem::deleteSound(Sound &sound)
{
    stop(sound);
    alGetError();
    alSourcei(sound.source, AL_BUFFER, 0);
    checkError("alSourcei");
    alDeleteSources(1, &sound.source);
    checkError("alDeleteSources");
}
void SoundSystem::init(GLuint entity)
{
    auto view{reg->view<Transform, Sound>()};
    auto [trans, sound]{view.get<Transform, Sound>(entity)};
    auto factory{ResourceManager::instance()};

    alcMakeContextCurrent(factory->context());
    alGetError();
    alGenSources(1, &sound.source);
    checkError("alGenSources");
    alSourcef(sound.source, AL_PITCH, 1.0f);
    alSourcef(sound.source, AL_GAIN, sound.gain);
    ALfloat temp[3] = {trans.position.x, trans.position.y, trans.position.z};
    alSourcefv(sound.source, AL_POSITION, temp);
    ALfloat temp2[3] = {sound.velocity.x, sound.velocity.y, sound.velocity.z};
    alSourcefv(sound.source, AL_VELOCITY, temp2);
    alSourcei(sound.source, AL_LOOPING, sound.looping);
    alSourcei(sound.source, AL_BUFFER, factory->getSoundBuffers()[sound.name]); // undefined behaviour if you try to use unloaded audio

    sound.initialized = true;
}

void SoundSystem::update(DeltaTime)
{
    updateListener();
    auto view{reg->view<Transform, Sound>()};
    for (auto entity : view) {
        auto [transform, sound]{view.get<Transform, Sound>(entity)};
        if (!sound.initialized) {
            init(entity);
        }
        if (sound.outDated) {
            if (sound.playing) {
                play(sound);
            }
            else if (sound.paused)
                pause(sound);
            else
                stop(sound);
            sound.outDated = false;
        }
        setPosition(entity, transform.position);
    }
}
void SoundSystem::play(Sound &sound)
{
    alSourcePlay(sound.source);
}
void SoundSystem::pause(Sound &sound)
{
    alSourcePause(sound.source);
}
void SoundSystem::stop(Sound &sound)
{
    alSourceStop(sound.source);
}
void SoundSystem::playAll()
{
    auto view{reg->view<Sound>()};
    for (auto entity : view) {
        play(entity);
    }
}
void SoundSystem::pauseAll()
{
    auto view{reg->view<Sound>()};
    for (auto entity : view) {
        pause(entity);
    }
}
void SoundSystem::stopAll()
{
    auto view{reg->view<Sound>()};
    for (auto entity : view) {
        stop(entity);
    }
}
void SoundSystem::play(GLuint eID)
{
    Sound &sound{reg->get<Sound>(eID)};
    sound.playing = true;
    sound.paused = false;
    sound.outDated = true;
}
void SoundSystem::pause(GLuint eID)
{
    Sound &sound{reg->get<Sound>(eID)};
    sound.playing = false;
    sound.paused = true;
    sound.outDated = true;
}
void SoundSystem::stop(GLuint eID)
{
    Sound &sound{reg->get<Sound>(eID)};
    sound.playing = false;
    sound.paused = false;
    sound.outDated = true;
}
void SoundSystem::setPosition(GLuint eID, vec3 newPos)
{
    Sound &sound{reg->get<Sound>(eID)};
    ALfloat temp[3] = {newPos.x, newPos.y, newPos.z};
    alSourcefv(sound.source, AL_POSITION, temp);
}
void SoundSystem::setVelocity(GLuint eID, vec3 newVel)
{
    Sound &sound{reg->get<Sound>(eID)};
    sound.velocity = newVel;
    ALfloat temp[3] = {newVel.x, newVel.y, newVel.z};
    alSourcefv(sound.source, AL_VELOCITY, temp);
}
void SoundSystem::updateListener()
{
    ALfloat posVec[3];
    ALfloat velVec[3];
    ALfloat headVec[6];
    ResourceManager *factory{ResourceManager::instance()};
    vec3 pos{factory->getCurrentCameraController()->cameraPosition()};
    posVec[0] = pos.x;
    posVec[1] = pos.y;
    posVec[2] = pos.z;
    vec3 vel{1}; // I guess this sets velocity to a constant 1 so we don't have to worry about it
    velVec[0] = vel.x;
    velVec[1] = vel.y;
    velVec[2] = vel.z;
    vec3 dir{factory->getCurrentCameraController()->forward()};
    headVec[0] = dir.x;
    headVec[1] = dir.y;
    headVec[2] = dir.z;
    vec3 up{factory->getCurrentCameraController()->up()};
    headVec[3] = up.x;
    headVec[4] = up.y;
    headVec[5] = up.z;
    alListenerfv(AL_POSITION, posVec);
    alListenerfv(AL_VELOCITY, velVec);
    alListenerfv(AL_ORIENTATION, headVec);
}
bool SoundSystem::checkError(std::string name)
{
    QString qname{QString::fromStdString(name)};
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
} // namespace cjk
