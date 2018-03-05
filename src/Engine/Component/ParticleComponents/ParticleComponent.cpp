#include "ParticleComponent.hpp"


ParticleComponent::ParticleComponent(GameObject & gameobject) :
    Component(gameobject)
{
    
}

void ParticleComponent::init() {
}   

void ParticleComponent::update(float dt) {

    for (auto & pe : activeEffects) {
        pe.update(dt);
    }
}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & pos) {
    ParticleEffect::EffectParams *ep = getEffectParams(effect);
    activeEffects.emplace_back(ep, pos);
}

ParticleEffect::EffectParams* ParticleComponent::getEffectParams(ParticleEffect::Effect effect) {

    switch (effect) {
    case ParticleEffect::Effect::BLOOD_SPLAT:
        return ParticleEffect::createEffectParams(ParticleEffect::Type::SPHERE,
            100, 5.0f, 0.0f, 0.0f, 2 * glm::pi<float>(), 100.0f, 1.0f, false,
            new glm::vec3(0), new glm::vec3(0.0f, 1.0f, 0.0f), NULL, NULL, NULL);
    case ParticleEffect::Effect::EXPLOADING_GUTS:
        return NULL;
    default:
        return NULL;
    }
        
}