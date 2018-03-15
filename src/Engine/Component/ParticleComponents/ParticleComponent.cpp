#include "ParticleComponent.hpp"

ParticleComponent::ParticleComponent(GameObject & gameobject) :
    Component(gameobject)
{  
}

ParticleComponent::ParticleComponent(GameObject & gameobject, SpatialComponent * anchor) :
    Component(gameobject),
    m_anchor(anchor)
{
}

ParticleComponent::ParticleComponent(GameObject & gameobject, SpatialComponent * anchor, SpatialComponent * directional) :
    Component(gameobject),
    m_anchor(anchor),
    m_directional(directional)
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
    if (m_anchor != NULL) {
        if (m_directional == NULL) {
            m_activeEffect->update(dt, m_anchor->position());
        }
        else {
            m_activeEffect->update(dt, m_anchor->position(), -m_directional->w());
        }

    }
    else {
        m_activeEffect->update(dt);
    }
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
        case ParticleEffect::Effect::BLOOD_SPRAY: {
            ParticleEffect::Type type = ParticleEffect::Type::CONE;
            int n = 2000;
            float effectDuration = 5.0f;
            float particleDuration = 0.5f;
            int orientations = 0;
            bool randomDistribution = true; 
            float variance = 0.1f;
            float rate = 1000.0f;
            float angle = glm::pi<float>()/24; 
            bool loop = false;
            float magnitude = 25.0f;
            float attenuation = 1.0f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return ParticleEffect::createEffectParams(type,n, effectDuration, particleDuration, orientations,
                randomDistribution, variance, rate, angle, loop, magnitude, attenuation, accelerators, meshes, textures);
        }
        case ParticleEffect::Effect::WATER_FOUNTAIN: {
            ParticleEffect::Type type = ParticleEffect::Type::CONE;
            int n = 600;
            float effectDuration = 600.0f;
            float particleDuration = 3.0f;
            int orientations = 0;
            bool randomDistribution = true; 
            float variance = 0.0f;
            float rate = 300.0f;
            float angle = glm::pi<float>() / 8; 
            bool loop = true;
            float magnitude = 5.0f;
            float attenuation = 1.0f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            accelerators->push_back(SpatialSystem::gravity());
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return ParticleEffect::createEffectParams(type, n, effectDuration, particleDuration, orientations,
                randomDistribution, variance, rate, angle, loop, magnitude, attenuation, accelerators, meshes, textures);
        }
        case ParticleEffect::Effect::SODA_GRENADE: {
            ParticleEffect::Type type = ParticleEffect::Type::SPHERE;
            int n = 200;
            float effectDuration = 3.0f;
            float particleDuration = 3.0f;
            int orientations = 20;
            bool randomDistribution = true; 
            float variance = 0.2f;
            float rate = 0.0f;
            float angle = glm::pi<float>() / 4; 
            bool loop = false;
            float magnitude = 80.0f;
            float attenuation = 0.8f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            accelerators->push_back(SpatialSystem::gravity());
            SpatialSystem::gravity();
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return ParticleEffect::createEffectParams(type, n, effectDuration, particleDuration, orientations,
                randomDistribution, variance, rate, angle, loop, magnitude, attenuation, accelerators, meshes, textures);
        }
        case ParticleEffect::Effect::BODY_EXPLOSION: {
            ParticleEffect::Type type = ParticleEffect::Type::SPHERE;
            int n = 40;
            float effectDuration = 1.5f;
            float particleDuration = 1.5f;
            int orientations = 20;
            bool randomDistribution = true; 
            float variance = 0.0f;
            float rate = 0.0f;
            float angle = glm::pi<float>() / 4; 
            bool loop = false;
            float magnitude = 50.0f;
            float attenuation = 0.85f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            accelerators->push_back(SpatialSystem::gravity());
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return ParticleEffect::createEffectParams(type, n, effectDuration, particleDuration, orientations,
                randomDistribution, variance, rate, angle, loop, magnitude, attenuation, accelerators, meshes, textures);
        }
        case ParticleEffect::Effect::SIRACHA_FLAMETHROWER: {
            ParticleEffect::Type type = ParticleEffect::Type::CONE;
            int n = 2000;
            float effectDuration = 5.0f;
            float particleDuration = 2.0f;
            int orientations = 0;
            bool randomDistribution = true; 
            float variance = 0.0f;
            float rate = 500.0f;
            float angle = glm::pi<float>() / 16; 
            bool loop = false;
            float magnitude = 40.0f;
            float attenuation = 1.0f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            accelerators->push_back(SpatialSystem::gravity());
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return ParticleEffect::createEffectParams(type, n, effectDuration, particleDuration, orientations,
                randomDistribution, variance, rate, angle, loop, magnitude, attenuation, accelerators, meshes, textures);
        }
        default:
            return NULL;
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
        case ParticleEffect::Effect::BLOOD_SPRAY:
        {
            meshes->push_back(Loader::getMesh("particles/Blood_Drop.obj"));
            return meshes;
        }
        case ParticleEffect::Effect::WATER_FOUNTAIN:
        {
            meshes->push_back(Loader::getMesh("particles/Water_Drop.obj"));
            return meshes;
        }
        case ParticleEffect::Effect::SODA_GRENADE:
        {
            meshes->push_back(Loader::getMesh("Hamburger.obj"));
            return meshes;
        }
        case ParticleEffect::Effect::BODY_EXPLOSION:
        {
            meshes->push_back(Loader::getMesh("particles/Gore_Chunk.obj"));
            return meshes;
        }
        case ParticleEffect::Effect::SIRACHA_FLAMETHROWER:
        {
            meshes->push_back(Loader::getMesh("particles/Blood_Drop.obj"));
            return meshes;
        }
        default:
            return NULL;
    }
}

Vector<ModelTexture*> * ParticleComponent::getTextures(ParticleEffect::Effect effect) {
    Vector<ModelTexture*> * textures = new Vector<ModelTexture*>();
    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPRAY:
        {
            Texture * tex(Loader::getTexture("particles/Blood_Drop_Tex.png"));
            textures->push_back(new ModelTexture(tex));
            return textures;
        }
        case ParticleEffect::Effect::WATER_FOUNTAIN:
        {
            Texture * tex(Loader::getTexture("particles/Water_Drop_Tex.png"));
            textures->push_back(new ModelTexture(tex));
            return textures;
        }
        case ParticleEffect::Effect::SODA_GRENADE:
        {
            Texture * tex(Loader::getTexture("Hamburger_BaseColor.png"));
            textures->push_back(new ModelTexture(tex));
            return textures;
        }
        case ParticleEffect::Effect::BODY_EXPLOSION:
        {
            Texture * tex(Loader::getTexture("particles/Gore_Chunk_Tex.png"));
            textures->push_back(new ModelTexture(tex));
            return textures;
        }
        case ParticleEffect::Effect::SIRACHA_FLAMETHROWER:
        {
            Texture * tex(Loader::getTexture("particles/Blood_Drop_Tex.png"));
            textures->push_back(new ModelTexture(tex));
            return textures;
        }
        default :
            return NULL;
    }
}

//  TO DO: Multi Mesh scale support
float ParticleComponent::getScaleFactor(ParticleEffect::Effect effect) {
    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPRAY:
        {
            return 0.6f;
        }
        case ParticleEffect::Effect::WATER_FOUNTAIN:
        {
            return 0.5f;
        }
        case ParticleEffect::Effect::SODA_GRENADE:
        {
            return 0.1f;
        }
        case ParticleEffect::Effect::BODY_EXPLOSION:
        {
            return 1.0f;
        }
        case ParticleEffect::Effect::SIRACHA_FLAMETHROWER:
        {
            return 1.0f;
        }
        default:
            return NULL;
    }
}