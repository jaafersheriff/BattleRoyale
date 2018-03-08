#pragma once
#include "Component/Component.hpp"
#include "ParticleEffect.hpp"
#include "Particle.hpp"
//#include "Shaders/ParticleShader.hpp"
#include "Loader/Loader.hpp"
#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"

class ParticleComponent : public Component {

    friend Scene;
    //friend ParticleRenderComponent;
    //friend ParticleShader;

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
        Mesh* getMesh(int i);
        ModelTexture* getModelTexture(int i);

    private:
        ParticleEffect::EffectParams* getEffectParams(ParticleEffect::Effect effect);
        Vector<Mesh*> * ParticleComponent::getMeshes(ParticleEffect::Effect effect);
        Vector<ModelTexture*> * ParticleComponent::getTextures(ParticleEffect::Effect effect);
    
    private:
        ParticleEffect *activeEffect = NULL;

};