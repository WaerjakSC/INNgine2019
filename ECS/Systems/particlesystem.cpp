#include "particlesystem.h"
#include "cameracontroller.h"
#include "inputsystem.h"
#include "movementsystem.h"
#include "particleshader.h"
#include "registry.h"

#include <QColor>
ParticleSystem::ParticleSystem(Ref<ParticleShader> shader)
    : registry{Registry::instance()}, mShader{shader} {
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
        if (emitter.shouldEmit) {
            generateParticles(deltaTime, emitter, transform);
            simulateParticles(deltaTime, emitter);
            renderParticles(emitter);
        }
    }
}

void ParticleSystem::generateParticles(DeltaTime deltaTime, ParticleEmitter &emitter, const Transform &transform) {
    int pps{emitter.particlesPerSecond};
    int newParticles{static_cast<int>(deltaTime * pps * 10)};
    //    int clamped{static_cast<int>(0.016f * pps)};
    //    if (newParticles > clamped)
    //        newParticles = clamped;
    for (int i{0}; i < newParticles; i++) {
        int particleIndex{findUnusedParticle(emitter)};
        Particle &particle{emitter.particlesContainer[particleIndex]};

        particle.life = emitter.lifeSpan;
        particle.position = transform.localPosition + vec3{0, 0.5f, 0};

        float spread{emitter.spread};
        vec3 mainDir{emitter.initialDirection};

        // Very bad way to generate a random direction;
        // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
        // combined with some user-controlled parameters (main direction, spread, etc)
        vec3 randomDir{(std::rand() % 2000 - 1000.0f) / 1000.0f,
                       (std::rand() % 2000 - 1000.0f) / 1000.0f,
                       (std::rand() % 2000 - 1000.0f) / 1000.0f};

        particle.velocity = mainDir + randomDir * spread;
        particle.r = emitter.initialColor.red();
        particle.g = emitter.initialColor.green();
        particle.b = emitter.initialColor.blue();
        particle.a = emitter.initialColor.alpha();

        // could do something like change the color with an affector or something here
    }
}
void ParticleSystem::simulateParticles(DeltaTime deltaTime, ParticleEmitter &emitter) {
    auto input{registry->system<InputSystem>()};

    emitter.activeParticles = 0;
    for (size_t i{0}; i < emitter.numParticles; i++) {

        Particle &particle{emitter.particlesContainer[i]};
        if (particle.life > 0.0f) {
            particle.life -= deltaTime;
            if (particle.life > 0.0f) {
                particle.velocity += vec3{0.0f, -9.81f, 0.0f} * deltaTime * emitter.speed;
                particle.position += particle.velocity * deltaTime;
                particle.cameraDistance = (particle.position - input->currentGameCameraController()->cameraPosition()).length();

                // fill the gpu buffer
                emitter.particlePositionData[4 * emitter.activeParticles + 0] = particle.position.x;
                emitter.particlePositionData[4 * emitter.activeParticles + 1] = particle.position.y;
                emitter.particlePositionData[4 * emitter.activeParticles + 2] = particle.position.z;
                emitter.particlePositionData[4 * emitter.activeParticles + 3] = emitter.size;

                emitter.particleColorData[4 * emitter.activeParticles + 0] = particle.r;
                emitter.particleColorData[4 * emitter.activeParticles + 1] = particle.g;
                emitter.particleColorData[4 * emitter.activeParticles + 2] = particle.b;
                emitter.particleColorData[4 * emitter.activeParticles + 3] = particle.a;
            } else {
                particle.cameraDistance = -1.f;
            }
            emitter.activeParticles++;
        }
    }
    sortParticles(emitter);
}
void ParticleSystem::sortParticles(ParticleEmitter &emitter) {
    std::sort(&emitter.particlesContainer[0], &emitter.particlesContainer[emitter.numParticles]);
}

void ParticleSystem::renderParticles(ParticleEmitter &emitter) {
    glBindBuffer(GL_ARRAY_BUFFER, emitter.particlePositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, emitter.numParticles * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, emitter.activeParticles * sizeof(GLfloat) * 4, emitter.particlePositionData.data());

    glBindBuffer(GL_ARRAY_BUFFER, emitter.particleColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, emitter.numParticles * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, emitter.activeParticles * sizeof(GLubyte) * 4, emitter.particleColorData.data());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mShader->use();

    mShader->transmitParticleUniformData(emitter);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, emitter.quadVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, emitter.particlePositionBuffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, emitter.particleColorBuffer);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (GLvoid *)0);

    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, emitter.numParticles);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisable(GL_BLEND);
}

int ParticleSystem::findUnusedParticle(ParticleEmitter &emitter) {
    int &lastParticle{emitter.lastUsedParticle};
    for (size_t i{static_cast<size_t>(lastParticle)}; i < emitter.numParticles; i++) {
        if (emitter.particlesContainer[i].life < 0) {
            lastParticle = i;
            return i;
        }
    }

    for (int i{0}; i < lastParticle; i++) {
        if (emitter.particlesContainer[i].life <= 0.0f) {
            lastParticle = i;
            return i;
        }
    }
    lastParticle = 0;
    return 0; // All particles are taken, override the first one
}
