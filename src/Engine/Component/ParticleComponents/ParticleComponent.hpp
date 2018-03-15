#pragma once
#include "util/Memory.hpp"
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

class ParticleSystem;

class ParticleComponent : public Component {

    friend Scene;
    friend ParticleSystem;

    protected:
        ParticleComponent(GameObject & gameobject);
        ParticleComponent(GameObject & gameobject, SpatialComponent * anchor);
        ParticleComponent(GameObject & gameobject, SpatialComponent * anchor, SpatialComponent * directional);

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
        Vector<int> * getParticleOrientationIDs();
        int Count() { return m_activeEffect->Count(); }
        float Dead() {return m_activeEffect->Life() > m_activeEffect->Duration();}
        Vector<glm::mat4> RandomMs() { return m_randomMs; }
        Vector<glm::mat3> RandomNs() { return m_randomNs; }
        unsigned int  OffsetBufferID() { return m_offsetBufferID; }
        unsigned int OrientationBufferID() { return m_orientationBufferID;  }
        

    private:
        ParticleEffect::EffectParams* getEffectParams(ParticleEffect::Effect effect);
        Vector<Mesh*> * ParticleComponent::getMeshes(ParticleEffect::Effect effect);
        Vector<ModelTexture*> * ParticleComponent::getTextures(ParticleEffect::Effect effect);
        void initRandomOrientations(glm::mat4 comp, glm::mat3 norm, int count);
        float getScaleFactor(ParticleEffect::Effect effect);

    
    public:
        static const int MAX_ORIENTATIONS = 20; // if changed, need to change vertex shader

    private:
        ParticleEffect *m_activeEffect;
        ParticleEffect::Effect m_effect;
        glm::mat4 m_M;
        glm::mat3 m_N;
        Vector<glm::mat4> m_randomMs;
        Vector<glm::mat3> m_randomNs;
        GLuint m_offsetBufferID;
        GLuint m_orientationBufferID;
        SpatialComponent * m_anchor = NULL;
        SpatialComponent * m_directional = NULL;
        
};