#include "ParticleComponent.hpp"

ParticleComponent::ParticleComponent(GameObject & gameobject) :
    Component(gameobject)
{  
}

void ParticleComponent::init() {
    const glm::fmat3 & rotation = glm::fmat3();
    const glm::fvec3 & translation = glm::fvec3(0.0f);
    const glm::fvec3 & scale = glm::fvec3(getScaleFactor(m_effect));
    glm::mat4 comp = Util::compositeTransform(scale, translation);
    m_M = comp;
    m_N = glm::mat3();

    initRandomOrientations(comp, m_N, m_activeEffect->getOrientationCount());
}   

void ParticleComponent::update(float dt) {  
   m_activeEffect->update(dt);
}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position, const glm::vec3 & direction, 
    const glm::vec3 & velocity) {
    ParticleEffect::EffectParams *effectParams = getEffectParams(effect);
    m_activeEffect = new ParticleEffect(effectParams, position, direction, velocity);
    m_effect = effect;
}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position, const glm::vec3 & direction) {
    ParticleEffect::EffectParams *effectParams = getEffectParams(effect);
    m_activeEffect = new ParticleEffect(effectParams, position, direction);
    m_effect = effect;

}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & position) {
    ParticleEffect::EffectParams *effectParams = getEffectParams(effect);
    m_activeEffect = new ParticleEffect(effectParams, position);
    m_effect = effect;
}

ParticleEffect::EffectParams* ParticleComponent::getEffectParams(ParticleEffect::Effect effect) {

    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT: {
            ParticleEffect::Type type = ParticleEffect::Type::SPHERE;
            int n = 2000;
            float effectDuration = 5.0f;
            float particleDuration = 2.0f;
            int orientations = 20;
            bool randomDistribution = true; // Have a random distribution vs uniform distribution
            float variance = 0.0f;
            float rate = 100.0f;
            float angle = glm::pi<float>()/8; // For Cones, must be limited to 0 -> pi
            bool loop = true;
            float magnitude = 10.0f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            accelerators->push_back(glm::vec3(0.0f, -9.8f, 0.0f));
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return ParticleEffect::createEffectParams(type,n, effectDuration, particleDuration, orientations,
                randomDistribution, variance, rate, angle, loop, magnitude, accelerators, meshes, textures);
        }
    }
        
}

Vector<glm::vec3> * ParticleComponent::getParticlePositions() {
    return m_activeEffect->ActiveParticlePositions();
}

Vector<int> * ParticleComponent::getParticleOrientationIDs() {
    return m_activeEffect->ActiveParticleOrientationIDs();
}

void ParticleComponent::initRandomOrientations(glm::mat4 comp, glm::mat3 norm, int count) {
    m_randomMs = Vector<glm::mat4>();
    m_randomMs.push_back(comp);
    m_randomNs = Vector<glm::mat3>();
    m_randomNs.push_back(norm);
    for (int i = 0; i < count; i++) {
        float angle = 2 * glm::pi<float>() * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        glm::fvec3 & axis = glm::ballRand(1.0f);

        const glm::fmat4 & rot = glm::rotate(angle, axis);
        const glm::fvec3 & translation = glm::fvec3(0.0f);
        const glm::fvec3 & scale = glm::fvec3(getScaleFactor(m_effect));
        m_randomNs.emplace_back(glm::mat3(rot) * glm::mat3(glm::scale(glm::mat4(), 1.0f / scale)));
        //m_randomNs.emplace_back(norm);
        m_randomMs.emplace_back(Util::compositeTransform(scale, rot, translation));
    }
}

Mesh* ParticleComponent::getMesh(int i) {
    return m_activeEffect->getMesh(i);
}

ModelTexture* ParticleComponent::getModelTexture(int i) {
    return m_activeEffect->getModelTexture(i);
}

Vector<Mesh*> * ParticleComponent::getMeshes(ParticleEffect::Effect effect) {
    Vector<Mesh*> * meshes = new Vector<Mesh*>();
    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT:
        {
            meshes->push_back(Loader::getMesh("Hamburger.obj"));
            return meshes;
        }
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
    }
}

//  TO DO: Multi Mesh scale support
float ParticleComponent::getScaleFactor(ParticleEffect::Effect effect) {
    switch (effect) {
    case ParticleEffect::Effect::BLOOD_SPLAT:
    {
        return 0.1f;
    }
    }
}