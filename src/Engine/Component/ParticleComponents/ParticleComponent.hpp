#pragma once
#include "Component/Component.hpp"
#include "ParticleEffect.hpp"
#include "Particle.hpp"

class ParticleComponent : Component {

    protected:
        ParticleComponent(GameObject & gameobject);

    public:
        void init();
        void update(float dt);
        void spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & pos);

    private:
        ParticleEffect::EffectParams* getEffectParams(ParticleEffect::Effect effect);
    
    private:
        Vector<ParticleEffect> activeEffects;

};