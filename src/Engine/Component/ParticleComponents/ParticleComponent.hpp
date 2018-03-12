#pragma once
#include "Component/Component.hpp"
#include "ParticleEffect.hpp"
#include "Loader/Loader.hpp"
#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"
#include "Util/Util.hpp"

class ParticleComponent : public Component {

    friend Scene;

    protected:
        ParticleComponent(GameObject & gameobject);

    public:
        ParticleComponent(ParticleComponent && other) = default;
    
    protected:    
        virtual void init() override;
    
    public:
        virtual SystemID systemID() const override { return SystemID::particle;  }
        void update(float dt) override;
        void spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & pos, const glm::vec3 & dir);
        Mesh* getMesh(int i);
        ModelTexture* getModelTexture(int i);
        glm::mat4 ModelMatrix() { return m_M; }
        glm::mat4 NormalMatrix() { return m_N; }
        Vector<glm::vec3> * getParticlePositions();
        int Count() { return activeEffect->Count(); }
        

    private:
        ParticleEffect::EffectParams* getEffectParams(ParticleEffect::Effect effect);
        Vector<Mesh*> * ParticleComponent::getMeshes(ParticleEffect::Effect effect);
        Vector<ModelTexture*> * ParticleComponent::getTextures(ParticleEffect::Effect effect);
    
    private:
        ParticleEffect * activeEffect;
        glm::mat4 m_M;
        glm::mat3 m_N;

};