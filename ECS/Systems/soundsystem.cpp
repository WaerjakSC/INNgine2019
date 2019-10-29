#include "soundsystem.h"
#include "camera.h"
#include "registry.h"
#include "resourcemanager.h"
SoundSystem::SoundSystem() : reg(Registry::instance()),
                             mDevice(NULL),
                             mContext(NULL) {
}

bool SoundSystem::createContext() {
    qDebug() << "Intializing OpenAL!\n";
    mDevice = alcOpenDevice(NULL);
    if (mDevice) {
        mContext = alcCreateContext(mDevice, NULL);
        alcMakeContextCurrent(mContext);
    }

    // Generate buffers
    alGetError();

    if (!mDevice) {
        qDebug() << "Device not made!\n";
    } else
        qDebug() << "Intialization complete!\n";
    return true;
}
void SoundSystem::cleanUp() {
    auto view = reg->view<Sound>();
    for (auto entity : view) {
        Sound &sound = view.get(entity);
        qDebug() << "Destroying SoundSource " + QString::fromStdString(sound.mName) + "\n";
        stop(sound);
        alGetError();
        alSourcei(sound.mSource, AL_BUFFER, 0);
        checkError("alSourcei");
        alDeleteSources(1, &sound.mSource);
        checkError("alDeleteSources");
        alDeleteBuffers(1, &sound.mBuffer);
        checkError("alDeleteBuffers");
    }
    mContext = alcGetCurrentContext();
    mDevice = alcGetContextsDevice(mContext);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(mContext);
    alcCloseDevice(mDevice);
}
void SoundSystem::init() {
    auto view = reg->view<Sound, Transform>();
    for (auto entity : view) {
        auto [trans, sound] = view.get<Transform, Sound>(entity);
        ALfloat temp[3] = {trans.position.x, trans.position.y, trans.position.z};
        alSourcefv(sound.mSource, AL_POSITION, temp);
        ALfloat temp2[3] = {sound.mVelocity.x, sound.mVelocity.y, sound.mVelocity.z};
        alSourcefv(sound.mSource, AL_VELOCITY, temp2);

        alSourcei(sound.mSource, AL_LOOPING, sound.mLooping);
        ResourceManager::instance()->loadWave(gsl::soundFilePath + sound.mName, sound);
    }
    //Start listing of found sound devices:
    //Not jet implemented
    //ALDeviceList *pDeviceList = NULL;
    //ALCcontext *pContext = NULL;
    //ALCdevice *pDevice = NULL;
    //ALint i;	//will hold the number of the preferred device
    //ALboolean bReturn = AL_FALSE;
}

void SoundSystem::update(float deltaTime) {
    Q_UNUSED(deltaTime);
    updateListener();
    auto view = reg->view<Transform, Sound>();
    for (auto entity : view) {
        auto [transform, sound] = view.get<Transform, Sound>(entity);
        if (sound.mOutDated) {
            if (sound.mPlaying) {
                play(sound);
                //            sound.mPlay = false; ???
            } else if (sound.mPaused)
                pause(sound);
            else
                stop(sound);
            sound.mOutDated = false;
        }
        setPosition(entity, transform.position);
    }
}
void SoundSystem::play(Sound &sound) {
    alSourcePlay(sound.mSource);
}
void SoundSystem::pause(Sound &sound) {
    alSourcePause(sound.mSource);
}
void SoundSystem::stop(Sound &sound) {
    alSourceStop(sound.mSource);
}
void SoundSystem::playAll() {
    auto view = reg->view<Sound>();
    for (auto entity : view) {
        play(entity);
    }
}
void SoundSystem::pauseAll() {
    auto view = reg->view<Sound>();
    for (auto entity : view) {
        pause(entity);
    }
}
void SoundSystem::stopAll() {
    auto view = reg->view<Sound>();
    for (auto entity : view) {
        stop(entity);
    }
}
void SoundSystem::play(GLuint eID) {
    Sound &sound = reg->getComponent<Sound>(eID);
    sound.mPlaying = true;
    sound.mPaused = false;
    sound.mOutDated = false;
}
void SoundSystem::pause(GLuint eID) {
    Sound &sound = reg->getComponent<Sound>(eID);
    sound.mPlaying = false;
    sound.mPaused = true;
    sound.mOutDated = false;
}
void SoundSystem::stop(GLuint eID) {
    Sound &sound = reg->getComponent<Sound>(eID);
    sound.mPlaying = false;
    sound.mPaused = false;
    sound.mOutDated = false;
}
void SoundSystem::setPosition(GLuint eID, vec3 newPos) {
    Sound &sound = reg->getComponent<Sound>(eID);
    ALfloat temp[3] = {newPos.x, newPos.y, newPos.z};
    alSourcefv(sound.mSource, AL_POSITION, temp);
}
void SoundSystem::setVelocity(GLuint eID, vec3 newVel) {
    Sound &sound = reg->getComponent<Sound>(eID);
    sound.mVelocity = newVel;
    ALfloat temp[3] = {newVel.x, newVel.y, newVel.z};
    alSourcefv(sound.mSource, AL_VELOCITY, temp);
}
void SoundSystem::updateListener() {
    ALfloat posVec[3];
    ALfloat velVec[3];
    ALfloat headVec[6];
    ResourceManager *factory = ResourceManager::instance();
    vec3 pos = factory->getCurrentCamera()->position();
    posVec[0] = pos.x;
    posVec[1] = pos.y;
    posVec[2] = pos.z;
    vec3 vel = vec3(1);
    velVec[0] = vel.x;
    velVec[1] = vel.y;
    velVec[2] = vel.z;
    vec3 dir = factory->getCurrentCamera()->forward();
    headVec[0] = dir.x;
    headVec[1] = dir.y;
    headVec[2] = dir.z;
    vec3 up = factory->getCurrentCamera()->up();
    headVec[3] = up.x;
    headVec[4] = up.y;
    headVec[5] = up.z;
    alListenerfv(AL_POSITION, posVec);
    alListenerfv(AL_VELOCITY, velVec);
    alListenerfv(AL_ORIENTATION, headVec);
}
bool SoundSystem::checkError(std::string name) {
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
