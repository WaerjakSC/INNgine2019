#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H
#include "gltypes.h"
#include "isystem.h"

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif
namespace gsl {
class Vector3D;
}
struct Sound;
class Registry;
class SoundSystem : public ISystem {
    using vec3 = gsl::Vector3D;

public:
    SoundSystem();
    void cleanUp();
    void init(GLuint entity);
    void update(DeltaTime = 0.016) override;
    void updatePlayOnly();

    void setPosition(GLuint eID, vec3 newPos);
    void setVelocity(GLuint eID, vec3 newVel);

    void playAll();
    void pauseAll();
    void stopAll();

    void deleteSound(Sound &sound);

private:
    Registry *registry;

    void play(Sound &sound);
    void pause(Sound &sound);
    void stop(Sound &sound);
    void updateListener();
    void refreshSounds();
    bool checkError(std::string name);
    friend class ResourceManager;
};

#endif // SOUNDSYSTEM_H
