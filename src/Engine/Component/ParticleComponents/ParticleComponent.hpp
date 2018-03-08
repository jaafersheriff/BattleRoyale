#pragma once
#include "Component/Component.hpp"
#include "ParticleEffect.hpp"
#include "Particle.hpp"

class ParticleComponent : public Component {

    friend Scene;

    protected:
        ParticleComponent(GameObject & gameobject);

    public:
        ParticleComponent(ParticleComponent && other) = default;
    
    protected:    
        virtual void init() override;
    
    public:
        virtual SystemID systemID() const override { return SystemID::gameLogic;  }
        void update(float dt) override;
        void spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & pos);

    private:
        ParticleEffect::EffectParams* getEffectParams(ParticleEffect::Effect effect);
    
    private:
        Vector<ParticleEffect> activeEffects;

};