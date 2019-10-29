#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H
#include "components.h"
#include "gltypes.h"
#include "isystem.h"
#include "vector3d.h"
#ifdef _WIN32
#include <al.h>
#include <alc.h>
#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif
typedef gsl::Vector3D vec3;
class Registry;
class SoundSystem : public ISystem {
public:
    SoundSystem();
    void cleanUp();
    void init();
    void update(float deltaTime = 0.016) override;

    void setPosition(GLuint eID, vec3 newPos);
    void setVelocity(GLuint eID, vec3 newVel);

    void play(GLuint eID);
    void pause(GLuint eID);
    void stop(GLuint eID);

    bool createContext();

    void playAll();
    void pauseAll();
    void stopAll();

private:
    Registry *reg;
    ALCdevice *mDevice{nullptr};   ///< Pointer to the ALC Device.
    ALCcontext *mContext{nullptr}; ///< Pointer to the ALC Context.
    void play(Sound &sound);
    void pause(Sound &sound);
    void stop(Sound &sound);
    void updateListener();
    bool checkError(std::string name);
    friend class Sound;
    friend class ResourceManager;
};

#endif // SOUNDSYSTEM_H
