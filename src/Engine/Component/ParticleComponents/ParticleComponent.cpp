#include "ParticleComponent.hpp"

ParticleComponent::ParticleComponent(GameObject & gameobject) :
    Component(gameobject)
{
    
}

void ParticleComponent::init() {
}   

void ParticleComponent::update(float dt) {
   activeEffect->update(dt);
}

void ParticleComponent::spawnParticleEffect(ParticleEffect::Effect effect, const glm::vec3 & pos, const glm::vec3 & dir) {
    ParticleEffect::EffectParams *ep = getEffectParams(effect);
    activeEffect = new ParticleEffect(ep, pos, dir);

}

ParticleEffect::EffectParams* ParticleComponent::getEffectParams(ParticleEffect::Effect effect) {

    switch (effect) {
    case ParticleEffect::Effect::BLOOD_SPLAT:
        return ParticleEffect::createEffectParams(ParticleEffect::Type::SPHERE,
            100, 5.0f, 0.0f, 0.0f, 2 * glm::pi<float>(), 100.0f, 1.0f, false,
            new glm::vec3(0), NULL, getMeshes(effect), getTextures(effect));
    default:
        return NULL;
    }
        
}

Mesh* ParticleComponent::getMesh(int i) {
    if (activeEffect != NULL) {
        return activeEffect->getMesh(i);
    }
    else {
        return NULL;
    }
}

ModelTexture* ParticleComponent::getModelTexture(int i) {
    if (activeEffect != NULL) {
        return activeEffect->getModelTexture(i);
    }
    else {
        return NULL;
    }
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