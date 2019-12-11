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
/**
 * @brief The SoundSystem class controls Sound components
 */
class SoundSystem : public ISystem {
    using vec3 = gsl::Vector3D;

public:
    SoundSystem();
    /**
     * @brief cleanUp for OpenAL called on destruction of RenderWindow
     */
    void cleanUp();
    void init(GLuint entity);
    void update(DeltaTime = 0.016) override;
    void updatePlayOnly();

    /**
     * @brief setPosition updates the position the sound play at for directional sound
     * @param eID
     * @param newPos
     */
    void setPosition(GLuint eID, vec3 newPos);
    void setVelocity(GLuint eID, vec3 newVel);

    void playAll();
    void pauseAll();
    void stopAll();

    /**
     * @brief deleteSound OpenAL cleanup for a removed sound component
     * @param sound
     */
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
