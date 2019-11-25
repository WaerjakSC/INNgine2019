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

void ParticleSystem::update(DeltaTime deltaTime) {
    auto view{registry->view<ParticleEmitter>()};
    initializeOpenGLFunctions();
    for (auto entity : view) {
        const auto &emitter{view.get(entity)};
        if (emitter.shouldEmit) {
            generateParticles(deltaTime, entity);
            simulateParticles(deltaTime, entity);
            renderParticles(entity);
        }
    }
}

void ParticleSystem::generateParticles(DeltaTime deltaTime, GLuint entityID) {
    auto view{registry->view<ParticleEmitter>()};
    auto &emitter{view.get(entityID)};
    // Generate 10 new particule each millisecond,
    // but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
    // newparticles will be huge and the next frame even longer.
    int pps{emitter.particlesPerSecond};
    int newParticles{static_cast<int>(deltaTime * pps)};
    int clamped{static_cast<int>(0.016f * pps)};
    if (newParticles > clamped)
        newParticles = clamped;
    for (int i{0}; i < newParticles; i++) {
        int particleIndex{findUnusedParticle(entityID)};
        Particle &particle{emitter.particlesContainer[particleIndex]};

        particle.life = emitter.lifeSpan;
        auto moveSys{registry->system<MovementSystem>()};
        particle.position = moveSys->getAbsolutePosition(entityID);

        float spread{emitter.spread};
        vec3 mainDir{emitter.initialDirection};

        // Very bad way to generate a random direction;
        // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
        // combined with some user-controlled parameters (main direction, spread, etc)
        vec3 randomDir{(std::rand() % 2000 - 1000.0f) / 1000.0f,
                       (std::rand() % 2000 - 1000.0f) / 1000.0f,
                       (std::rand() % 2000 - 1000.0f) / 1000.0f};

        particle.velocity = mainDir + randomDir * spread;
        particle.colorData[0] = emitter.initialColor.red();
        particle.colorData[1] = emitter.initialColor.blue();
        particle.colorData[2] = emitter.initialColor.green();
        particle.colorData[3] = emitter.initialColor.alpha();

        // could do something like change the color with an affector or something here
    }
}
void ParticleSystem::simulateParticles(DeltaTime deltaTime, GLuint entityID) {
    auto view{registry->view<ParticleEmitter>()};
    auto input{registry->system<InputSystem>()};
    auto &emitter{view.get(entityID)};

    emitter.activeParticles = 0;
    for (int i{0}; i < emitter.numParticles; i++) {

        Particle &particle{emitter.particlesContainer[i]};
        if (particle.life > 0.0f) {
            particle.life -= deltaTime;
            if (particle.life > 0.0f) {
                particle.velocity += vec3{0.0f, -9.81f, 0.0f} * deltaTime * emitter.speed;
                particle.position += particle.velocity * deltaTime;
                particle.cameraDistance = (particle.position - input->currentGameCameraController()->cameraPosition()).length();

                // fill the gpu buffer
                particle.positionSizeData[0] = particle.position.x;
                particle.positionSizeData[1] = particle.position.y;
                particle.positionSizeData[2] = particle.position.z;

                particle.positionSizeData[3] = emitter.size;
                // could set the color here?
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

void ParticleSystem::renderParticles(GLuint entityID) {
    auto view{registry->view<ParticleEmitter>()};
    auto &emitter{view.get(entityID)};

    glBindBuffer(GL_ARRAY_BUFFER, emitter.particlePositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, emitter.numParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, emitter.activeParticles * sizeof(GLfloat) * 4, emitter.particlesContainer.data()->positionSizeData);

    glBindBuffer(GL_ARRAY_BUFFER, emitter.particleColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, emitter.numParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, emitter.activeParticles * sizeof(GLubyte) * 4, emitter.particlesContainer.data()->colorData);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(mShader->getProgram());
    mShader->transmitParticleUniformData(emitter);

    glBindVertexArray(emitter.VAO);
    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

    // Draw the particles!
    // This draws many small triangle strips (which looks like a quad).
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, emitter.activeParticles);

    glBindVertexArray(0);
}

int ParticleSystem::findUnusedParticle(GLuint entityID) {
    auto view{registry->view<ParticleEmitter>()};
    auto &emitter{view.get(entityID)};
    int &lastParticle{emitter.lastUsedParticle};
    for (int i{lastParticle}; i < emitter.numParticles; i++) {
        if (emitter.particlesContainer[i].life < 0) {
            lastParticle = i;
            return i;
        }
    }

    for (int i{0}; i < lastParticle; i++) {
        if (emitter.particlesContainer[i].life < 0) {
            lastParticle = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}
