#pragma once
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/random.hpp"
#include "Component/Component.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"
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

        void spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position, 
            const glm::vec3 & direction, const glm::vec3 & velocity);
        void spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position,
            const glm::vec3 & direction);
        void spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position);
        
        Mesh* getMesh(int i);
        ModelTexture* getModelTexture(int i);
        glm::mat4 ModelMatrix() { return m_M; }
        glm::mat4 NormalMatrix() { return m_N; }
        Vector<glm::vec3> * getParticlePositions();
        int Count() { return activeEffect->Count(); }
        float Dead() {return activeEffect->Life() > activeEffect->Duration();}
        Vector<glm::mat4> RandomMs() { return m_randomMs; }
        

    private:
        ParticleEffect::EffectParams* getEffectParams(ParticleEffect::Effect effect);
        Vector<Mesh*> * ParticleComponent::getMeshes(ParticleEffect::Effect effect);
        Vector<ModelTexture*> * ParticleComponent::getTextures(ParticleEffect::Effect effect);
        Vector<glm::mat4> initRandomMs(int count);
    
    private:
        ParticleEffect* activeEffect;
        glm::mat4 m_M;
        glm::mat3 m_N;
        Vector<glm::mat4> m_randomMs;    

};