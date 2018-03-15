#include "ParticleSystem.hpp"

const Vector<ParticleComponent *> & ParticleSystem::s_particleComponents(Scene::getComponents<ParticleComponent>());
void ParticleSystem::init() {

}

void ParticleSystem::update(float dt) {
    for (auto &p : s_particleComponents) {
        p->update(dt);
        if (p->Dead()) {
            Scene::destroyGameObject(p->gameObject());
        }
    }
}
