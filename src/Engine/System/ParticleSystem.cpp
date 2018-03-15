#include "ParticleSystem.hpp"

#include "Scene/Scene.hpp"
#include "Component/ParticleComponents/ParticleComponent.hpp"

const Vector<ParticleComponent *> & ParticleSystem::s_particleComponents(Scene::getComponents<ParticleComponent>());

void ParticleSystem::init() {

}

void ParticleSystem::update(float dt) {
    for (ParticleComponent * p : s_particleComponents) {
        p->update(dt);
    }
}