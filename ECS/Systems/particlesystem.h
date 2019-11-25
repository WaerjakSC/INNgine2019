#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "isystem.h"
#include "pool.h"
#include <QOpenGLFunctions_4_1_Core>
struct Particle;
struct ParticleEmitter;
struct ParticleShader;
class Registry;
class ParticleSystem : public ISystem, public QOpenGLFunctions_4_1_Core {
public:
    ParticleSystem(Ref<ParticleShader> shader);

    void update(DeltaTime deltaTime = 0.016) override;

private:
    Registry *registry;
    Ref<ParticleShader> mShader;
    int findUnusedParticle(GLuint entityID);
    void generateParticles(DeltaTime deltaTime, GLuint entityID);
    void simulateParticles(DeltaTime deltaTime, GLuint entityID);
    void sortParticles(ParticleEmitter &emitter);
    void renderParticles(GLuint entityID);
};

#endif // PARTICLESYSTEM_H
