#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "core.h"
#include "isystem.h"
#include "vector3d.h"
#include <QOpenGLFunctions_4_1_Core>
#include <random>

class ParticleShader;
class Registry;
struct ParticleEmitter;
struct Transform;
/**
 * @brief The ParticleSystem class is in control of particles.
 */
class ParticleSystem : public QObject, public ISystem, public QOpenGLFunctions_4_1_Core {
    Q_OBJECT
    using vec3 = gsl::Vector3D;

public:
    ParticleSystem(cjk::Ref<ParticleShader> shader);
    /**
     * @brief Initializes any emitters that exist at renderwindow start.
     */
    void init();
    /**
     * @brief Initializes emitter to make it ready for use.
     * @param entityID
     */
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
    /**
     * @brief Finds dead particles and sets them ready for a new generation.
     * @param emitter
     * @return
     */
    int findUnusedParticle(ParticleEmitter &emitter);
    /**
     * @brief Generate new particles based on the settings of the ParticleEmitter.
     * @param deltaTime
     * @param emitter
     * @param transform Used to find the spawn location of the particles
     */
    void generateParticles(DeltaTime deltaTime, ParticleEmitter &emitter, const Transform &transform);
    /**
     * @brief Reduce the life of active particles by deltaTime, killing any that fall below 0 life.
     * Also sets the new position and color of active particles.
     * @param deltaTime
     * @param emitter
     */
    void simulateParticles(DeltaTime deltaTime, ParticleEmitter &emitter);
    /**
     * @brief Renders all active particles originating from emitter.
     * @param emitter
     */
    void renderParticles(ParticleEmitter &emitter);
    /**
     * @copydoc ParticleSystem::renderParticles(ParticleEmitter &)
     */
    void drawElements(ParticleEmitter &emitter);
};

#endif // PARTICLESYSTEM_H
