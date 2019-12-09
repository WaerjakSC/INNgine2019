#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "isystem.h"
#include "pool.h"
#include <QOpenGLFunctions_4_1_Core>
#include <random>

struct ParticleEmitter;
class ParticleShader;
class Registry;
class ParticleSystem : public QObject, public ISystem, public QOpenGLFunctions_4_1_Core {
    Q_OBJECT
    using vec3 = gsl::Vector3D;

public:
    ParticleSystem(cjk::Ref<ParticleShader> shader);
    void init();
    void initEmitter(GLuint entityID);

    void update(DeltaTime = 0.016) override;
    void updatePlayOnly(DeltaTime deltaTime = 0.016);
public slots:
    void setInitDirX(double xIn);
    void setInitDirY(double yIn);
    void setInitDirZ(double zIn);

    void setInitColorRed(int inRed);
    void setInitColorGreen(int inGreen);
    void setInitColorBlue(int inBlue);

    void setEmitterSpeed(double speed);
    void setEmitterSize(double size);

    void setEmitterLifeSpan(double lifespan);
    void setEmitterSpread(double spread);
    void setNumParticles(int num);
    void setPPS(int num);
    void setActiveEmitter(bool state);
    void setDecayEmitter(bool state);

private:
    Registry *registry;
    cjk::Ref<ParticleShader> mShader;
    std::mt19937 rng;
    int findUnusedParticle(ParticleEmitter &emitter);
    void generateParticles(DeltaTime deltaTime, ParticleEmitter &emitter, const Transform &transform);
    void simulateParticles(DeltaTime deltaTime, ParticleEmitter &emitter);
    void sortParticles(ParticleEmitter &emitter);
    void renderParticles(ParticleEmitter &emitter);
    void drawElements(ParticleEmitter &emitter);
};

#endif // PARTICLESYSTEM_H
