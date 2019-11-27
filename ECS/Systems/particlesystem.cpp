#include "particlesystem.h"
#include "cameracontroller.h"
#include "inputsystem.h"
#include "movementsystem.h"
#include "particleshader.h"
#include "registry.h"
#include <QColor>
ParticleSystem::ParticleSystem(Ref<ParticleShader> shader)
    : registry{Registry::instance()}, mShader{shader} {
    rng = std::mt19937(std::random_device()());
}

void ParticleSystem::init() {
    auto view{registry->view<ParticleEmitter, Transform>()};
    for (auto entity : view) {
        auto &emitter{view.get<ParticleEmitter>(entity)};
        if (!emitter.initialized) {
            ResourceManager::instance()->initParticleEmitter(emitter);
        }
    }
}
void ParticleSystem::initEmitter(GLuint entityID) {
    auto view{registry->view<ParticleEmitter, Transform>()};
    auto &emitter{view.get<ParticleEmitter>(entityID)};
    ResourceManager::instance()->initParticleEmitter(emitter);
}
void ParticleSystem::update(DeltaTime deltaTime) {
    auto view{registry->view<ParticleEmitter, Transform>()};
    initializeOpenGLFunctions();
    for (auto entity : view) {
        auto [emitter, transform]{view.get<ParticleEmitter, Transform>(entity)};
        if (emitter.isActive) {
            generateParticles(deltaTime, emitter, transform);
            simulateParticles(deltaTime, emitter);
            renderParticles(emitter);
            if (emitter.shouldDecay) { // if the emitter should only create a short burst of particles, we reduce the lifespan of each of its particles every frame.
                emitter.lifeSpan -= deltaTime;
                if (emitter.lifeSpan <= 0) {
                    emitter.isActive = false;
                    emitter.lifeSpan = emitter.initLifeSpan;
                }
            }
        }
    }
}

void ParticleSystem::generateParticles(DeltaTime deltaTime, ParticleEmitter &emitter, const Transform &transform) {
    int newParticles{static_cast<int>(deltaTime * 10 * emitter.particlesPerSecond)}; // Have to multiply by four because otherwise deltaTime is too small to cast to 1+
    for (int i{0}; i < newParticles; i++) {
        int particleIndex{findUnusedParticle(emitter)};
        Particle &particle{emitter.particles[particleIndex]};

        particle.life = emitter.lifeSpan;
        particle.position = transform.localPosition;

        float spread{emitter.spread};
        std::uniform_real_distribution<float> randomRadius(0, 1); // get a random radian point between 0 and 1
        vec3 mainDir{0};
        float d;
        do {
            mainDir.x = randomRadius(rng) * 2.0f - 1.0f;
            mainDir.y = randomRadius(rng) * 2.0f - 1.0f;
            mainDir.z = randomRadius(rng) * 2.0f - 1.0f;
            d = mainDir.x * mainDir.x + mainDir.y * mainDir.y + mainDir.z * mainDir.z;
        } while (d > 1.0f);
        // initial direction affects the general direction the particles will fly, especially if you don't turn on gravity.
        particle.velocity = mainDir + emitter.initialDirection * spread;
        particle.r = emitter.initialColor.red();
        particle.g = emitter.initialColor.green();
        particle.b = emitter.initialColor.blue();
        particle.a = emitter.initialColor.alpha();

        // could do something like change the color with an affector or something here
    }
}
void ParticleSystem::simulateParticles(DeltaTime deltaTime, ParticleEmitter &emitter) {

    int particleCount{0};
    for (size_t i{0}; i < emitter.numParticles; i++) {

        Particle &particle{emitter.particles[i]};

        if (particle.life > 0.0f) {
            particle.life -= deltaTime;
            if (particle.life > 0.0f) {
                //                particle.velocity += vec3{0.0f, -9.81f, 0.0f} * deltaTime * emitter.speed;
                particle.position += particle.velocity * deltaTime;

                // fill the gpu buffer
                emitter.positionData[4 * particleCount + 0] = particle.position.x;
                emitter.positionData[4 * particleCount + 1] = particle.position.y;
                emitter.positionData[4 * particleCount + 2] = particle.position.z;
                emitter.positionData[4 * particleCount + 3] = emitter.size;

                emitter.colorData[4 * particleCount + 0] = particle.r;
                emitter.colorData[4 * particleCount + 1] = particle.g;
                emitter.colorData[4 * particleCount + 2] = particle.b;
                if (emitter.shouldDecay) {
                    emitter.colorData[4 * particleCount + 3] = particle.a * gsl::clamp(particle.life, 0, 1);
                } else {
                    emitter.colorData[4 * particleCount + 3] = particle.a;
                }
                std::swap(emitter.particles[i], emitter.particles[particleCount]);
            }
            particleCount++;
        }
    }
    emitter.activeParticles = particleCount;
    // could sort the particles here for better particle transparency but not a priority and it takes too much performance
}

void ParticleSystem::renderParticles(ParticleEmitter &emitter) {
    glBindBuffer(GL_ARRAY_BUFFER, emitter.positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, emitter.numParticles * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, emitter.activeParticles * sizeof(GLfloat) * 4, emitter.positionData.data());

    glBindBuffer(GL_ARRAY_BUFFER, emitter.colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, emitter.numParticles * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, emitter.activeParticles * sizeof(GLubyte) * 4, emitter.colorData.data());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mShader->use();

    mShader->transmitParticleUniformData(emitter);
    drawElements(emitter);
}

void ParticleSystem::drawElements(ParticleEmitter &emitter) {
    glBindVertexArray(emitter.VAO);

    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, emitter.quadVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (GLvoid *)0);
    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, emitter.positionBuffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, emitter.colorBuffer);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (GLvoid *)0);

    glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr, emitter.activeParticles);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

int ParticleSystem::findUnusedParticle(ParticleEmitter &emitter) {
    int &lastParticle{emitter.lastUsedParticle};
    for (size_t i{static_cast<size_t>(lastParticle)}; i < emitter.numParticles; i++) {
        if (emitter.particles[i].life <= 0) {
            lastParticle = i;
            return i;
        }
    }

    for (int i{0}; i < lastParticle; i++) {
        if (emitter.particles[i].life <= 0.0f) {
            lastParticle = i;
            return i;
        }
    }
    lastParticle = 0;
    return 0; // All particles are taken, override the first one
}
