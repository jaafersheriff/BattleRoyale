#include "ParticleComponent.hpp"

ParticleComponent::ParticleComponent(GameObject & gameobject) :
    Component(gameobject)
{
    
}

void ParticleComponent::init() {
    glm::fvec3 & scalef = glm::fvec3(1.0f);
    glm::vec3 & scale = glm::vec3(1.0f);
    glm::fmat3 & rotf = glm::fmat3(0.0f);
    glm::mat3 & rot = glm::mat3(0.0f);
    glm::fvec3 & translatef = glm::fvec3(0.0f);
    glm::vec3 & translate = glm::vec3(0.0f);
    //m_M = Util::compositeTransform(scalef, rotf, translatef);
    //m_N = rot * glm::mat3(glm::scale(glm::mat4(), 1.0f / scale));
    m_M = glm::mat4();
    m_N = glm::mat3();
}   

void ParticleComponent::update(float dt) {
   activeEffect->update(dt);
}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position, const glm::vec3 & direction, 
    const glm::vec3 & velocity) {
    ParticleEffect::EffectParams *effectParams = getEffectParams(effect);
    activeEffect = new ParticleEffect(effectParams, position, direction, velocity);

}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position, const glm::vec3 & direction) {
    ParticleEffect::EffectParams *effectParams = getEffectParams(effect);
    activeEffect = new ParticleEffect(effectParams, position, direction);

}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position) {
    ParticleEffect::EffectParams *effectParams = getEffectParams(effect);
    activeEffect = new ParticleEffect(effectParams, position);

}

ParticleEffect::EffectParams* ParticleComponent::getEffectParams(ParticleEffect::Effect effect) {

    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT: {
            //UNIMPLEMENTED - CONAL, DISK, Rate, angle, maxDist
            //IMPLEMENTED - Speed, n, accelerators, loop, getMeshes, getTextures, variance
            int n = 100;
            float effectDuration = 5.0f;
            float particleDuration = 5.0f;
            float variance = 0.5f;
            float rate = 0.0f;
            float angle = 2 * glm::pi<float>();
            float maxDist = 100.0f;
            bool loop = false;
            float magnitude = 5.0f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            //accelerators->push_back(glm::vec3(0.0f, -100.0f, 0.0f));
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return ParticleEffect::createEffectParams(ParticleEffect::Type::SPHERE,
                n, effectDuration, particleDuration, variance, rate, angle, maxDist, loop, magnitude,
                 accelerators, meshes, textures);
        }
        default:
            return NULL;
    }
        
}

Vector<glm::vec3> * ParticleComponent::getParticlePositions() {
    return activeEffect->ActiveParticlePositions();
}

Mesh* ParticleComponent::getMesh(int i) {
    return activeEffect->getMesh(i);
}

ModelTexture* ParticleComponent::getModelTexture(int i) {
    return activeEffect->getModelTexture(i);
}

Vector<Mesh*> * ParticleComponent::getMeshes(ParticleEffect::Effect effect) {
    Vector<Mesh*> * meshes = new Vector<Mesh*>();
    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT:
        {
            meshes->push_back(Loader::getMesh("Hamburger.obj"));
            return meshes;
        }
        default:
            return NULL;
    }
}

Vector<ModelTexture*> * ParticleComponent::getTextures(ParticleEffect::Effect effect) {
    Vector<ModelTexture*> * textures = new Vector<ModelTexture*>();
    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT:
        {
            Texture * tex(Loader::getTexture("Hamburger_BaseColor.png"));
            textures->push_back(new ModelTexture(tex));
            return textures;
        }
         default:
            return NULL;
    }
}