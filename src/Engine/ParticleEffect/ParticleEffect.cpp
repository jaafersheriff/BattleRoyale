#include "ParticleEffect.hpp"

ParticleEffect::ParticleEffect(int type, int total, float duration, glm::vec3 origin) :
    type(type),
    total(total),
    duration(duration),
    origin(origin),
    life(0),
    particles(generateParticles()),
    tData(0.f)
{
}

std::vector<Particle> ParticleEffect::generateParticles() {
    std::vector<Particle> vp = std::vector<Particle>();
    for (int i = 0; i < total; i++) {
        //Particle p = Particle(type, i, total, position, duration);
        vp.emplace_back(type, i, total, origin);
        
    }
    return vp;
}

void ParticleEffect::update(float dt) {
    life += dt;
    tData = life / duration;
    if (life < duration) {
        for (int i = 0; i < total; i++) {
            particles[i].update(tData);
        }
    }
}