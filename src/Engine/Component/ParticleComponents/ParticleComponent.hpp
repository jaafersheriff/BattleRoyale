#pragma once
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/random.hpp"
#include "Component/Component.hpp"
#include "Component/SpatialComponents/PhysicsComponents.hpp"
#include "System/SpatialSystem.hpp"
#include "ParticleEffect.hpp"
#include "Loader/Loader.hpp"
#include "Model/Mesh.hpp"
#include "Model/ModelTexture.hpp"
#include "Util/Util.hpp"

class ParticleComponent : public Component {

    friend Scene;

    public:

        ParticleComponent(GameObject & gameObject, ParticleEffect::Effect effect);
        ParticleComponent(ParticleComponent && other) = default;
    
    protected:

        virtual void init() override;
    
    public:

        virtual SystemID systemID() const override { return SystemID::particle; }

        void update(float dt) override;

        void spawnParticleEffect(
            ParticleEffect::Effect effect,
            const glm::vec3 & position, 
            const glm::vec3 & direction,
            const glm::vec3 & velocity
        );
        void spawnParticleEffect(
            ParticleEffect::Effect effect,
            const glm::vec3 & position,
            const glm::vec3 & direction
        );
        void spawnParticleEffect(
            ParticleEffect::Effect effect,
            const glm::vec3 & position
        );
        
        Mesh * getMesh(int i);
        ModelTexture * getModelTexture(int i);
        Vector<glm::vec3> * getParticlePositions();
        Vector<int> * getParticleOrientationIDs();

        int count() { return m_activeEffect->count(); }
        float dead() {return m_activeEffect->life() > m_activeEffect->duration();}

        const glm::mat4 & modelMatrix() { return m_M; }
        const glm::mat3 & normalMatrix() { return m_N; }
        const Vector<glm::mat4> & randomMs() { return m_randomMs; }
        const Vector<glm::mat3> & randomNs() { return m_randomNs; }        

    private:

        ParticleEffect::EffectParams * getEffectParams(ParticleEffect::Effect effect);
        Vector<Mesh *> * ParticleComponent::getMeshes(ParticleEffect::Effect effect);
        Vector<ModelTexture *> * ParticleComponent::getTextures(ParticleEffect::Effect effect);
        void initRandomOrientations(const glm::mat4 & comp, const glm::mat3 & norm, int count);
        float getScaleFactor(ParticleEffect::Effect effect);
    
    public:

        static const int k_maxOrientations;

    private:

        ParticleEffect::Effect m_effect;
        UniquePtr<ParticleEffect> m_activeEffect;
        glm::mat4 m_M;
        glm::mat3 m_N;
        Vector<glm::mat4> m_randomMs;
        Vector<glm::mat3> m_randomNs;
        
};