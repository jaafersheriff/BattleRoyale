#include "ParticleSystem.hpp"

Vector<ParticleComponent *> & ParticleSystem::components = Vector<ParticleComponent *>();

void ParticleSystem::init() {

}

void ParticleSystem::update(float dt) {
    for (auto p : components) {
        p->update(dt);
    }
}