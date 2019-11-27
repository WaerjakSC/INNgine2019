#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "isystem.h"
#include "pool.h"
#include <QOpenGLFunctions_4_1_Core>
#include <random>

struct Particle;
struct ParticleEmitter;
struct ParticleShader;
class Registry;
class ParticleSystem : public ISystem, public QOpenGLFunctions_4_1_Core {
public:
    ParticleSystem(Ref<ParticleShader> shader);
    void init();
    void initEmitter(GLuint entityID);

    void update(DeltaTime deltaTime = 0.016) override;

private:
    Registry *registry;
    Ref<ParticleShader> mShader;
    std::mt19937 rng;
    int findUnusedParticle(ParticleEmitter &emitter);
    void generateParticles(DeltaTime deltaTime, ParticleEmitter &emitter, const Transform &transform);
    void simulateParticles(DeltaTime deltaTime, ParticleEmitter &emitter);
    void sortParticles(ParticleEmitter &emitter);
    void renderParticles(ParticleEmitter &emitter);
    void drawElements(ParticleEmitter &emitter);
};

#endif // PARTICLESYSTEM_H
