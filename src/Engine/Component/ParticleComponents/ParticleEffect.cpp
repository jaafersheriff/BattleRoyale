#include "ParticleEffect.hpp"

ParticleEffect::ParticleEffect() :
    m_effectParams(NULL),
    m_origin(NULL),
    m_particles(Vector<Particle*>()),
    m_life(0.0f)
{
}

ParticleEffect::ParticleEffect(EffectParams *effectParams, const glm::vec3 & origin) :
    m_effectParams(effectParams),
    m_origin(origin),
    m_direction(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_velocity(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_particles(generateParticles(effectParams)), 
    m_activeParticlePositions(getActiveParticlePositions()),
    m_activeMap(getActiveMap()),
    m_life(0.0f)
{
}

ParticleEffect::ParticleEffect(EffectParams *effectParams, const glm::vec3 & origin, const glm::vec3 & direction) :
    m_effectParams(effectParams),
    m_origin(origin),
    m_direction(direction),
    m_velocity(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_particles(generateParticles(effectParams)),
    m_activeParticlePositions(getActiveParticlePositions()),
    m_activeMap(getActiveMap()),
    m_life(0.0f)
{
}

ParticleEffect::ParticleEffect(EffectParams *effectParams, const glm::vec3 & origin, const glm::vec3 & direction, 
    const glm::vec3 & velocity) :
    m_effectParams(effectParams),
    m_origin(origin),
    m_direction(direction),
    m_velocity(velocity),
    m_particles(generateParticles(effectParams)),
    m_activeParticlePositions(getActiveParticlePositions()),
    m_activeMap(getActiveMap()),
    m_life(0.0f)
{
}

void ParticleEffect::update(float dt) {
    m_life += dt;

    //Active Online Particles
    updateActiveParticles(dt);

    if (m_life < m_effectParams->effectDuration) {
        for (int i = 0; i < m_activeParticlePositions->size(); i++) {
            Particle *p = m_particles[m_activeMap[i]];        
            updatePosition(p, dt);
            m_activeParticlePositions->at(i) = p->position;
            p->life += dt;

        }
    }
    // NOT UPDATED
    else if (m_effectParams->loop) {
        m_life = 0.0f;
        for (int i = 0; i < m_effectParams->n; i++) {
            m_particles[i]->position = m_origin;
            sphereMotion(m_particles[i], 1.0f);
        }
        update(dt);
    }
}

void ParticleEffect::updateActiveParticles(float dt) {
    
    if (m_effectParams->rate != 0.0f) {
        // Check if particle duration is expired and remove from list if so
        int i = 0;
        while(i < m_activeParticlePositions->size()) {
            Particle *p = m_particles[m_activeMap[i]];
            if (p->life > m_effectParams->particleDuration) {
                m_activeMap.erase(m_activeMap.begin() + i);
                m_activeParticlePositions->erase(m_activeParticlePositions->begin() + i);
            }
            else {
                i++;
            }
        }

        // Activate new particles based on rate - cap at n limit. Possibly remove the oldest


    }
}

void ParticleEffect::updatePosition(Particle *p, float dt) {
    for (int i = 0; i < m_effectParams->accelerators->size(); i++) {
        p->velocity += m_effectParams->accelerators->at(i) * dt;
    }
    p->position += p->velocity * dt;
}

//Spherical 
void ParticleEffect::sphereMotion(Particle* p, float speed) {
    float phi = glm::golden_ratio<float>();
    float z = 1 - (2 * float(p->i) / float(m_effectParams->n - 1));
    float radius = sqrt(1 - z * z);
    float theta = 2 * glm::pi<float>() * (2 - phi) * float(p->i);
    p->velocity = speed * glm::vec3(radius * cos(theta), radius * sin(theta), z);
}

Vector<ParticleEffect::Particle*> ParticleEffect::generateParticles(
    ParticleEffect::EffectParams *effectParams) {
    Vector<Particle*> vp = Vector<Particle*>();
    for (int i = 0; i < m_effectParams->n; i++) {
        Particle *p = new Particle();

        //General Particle Initialization
        initParticle(p, i, 0, 0);
        
        //Determine if active/inactive
        if (m_effectParams->rate == 0.0f) {
            p->active = true;

            //Initialize starting Position
            //UNIMPLEMENTED - Multi mesh/Textures
            initPosition(p);

            //initialize starting Velocity
            initVelocity(p);
        
        }
        
        vp.push_back(p);
        
    }
    return vp;
}

void ParticleEffect::initParticle(Particle *p, int i, int meshID, int modelTextureID) {
    p->i = i;
    p->meshID = meshID;
    p->modelTextureID = modelTextureID;
}

void ParticleEffect::initVelocity(Particle *p) {
    switch (m_effectParams->type) {
    case ParticleEffect::SPHERE:
        sphereMotion(p, m_effectParams->magnitude);
        
        break;
    default:
        p->velocity = glm::vec3(0.0f);
    }

    //Add random variance if specified
    if (m_effectParams->variance != 0.0f) {
        //glm::vec3 randVec = glm::vec3((rand() % 200 - 100) / 100.0f, 
        //    (rand() % 200 - 100) / 100.0f, (rand() % 200 - 100) / 100.0f);
        glm::vec3 randVec = glm::ballRand(m_effectParams->variance);
        p->velocity += randVec;
    }
}

void ParticleEffect::initPosition(Particle *p) {
    p->position = m_origin + m_life * m_velocity;
}

Vector<glm::vec3> * ParticleEffect::getActiveParticlePositions() {
    Vector<glm::vec3> *positions = new Vector<glm::vec3>();
    for (int i = 0; i < m_effectParams->n; i++){
        if (m_particles[i]->active) {
            positions->push_back(m_particles[i]->position);
        }
    }
    return positions;
}

Vector<int> ParticleEffect::getActiveMap() {
    Vector<int> map = Vector<int>();
    for (int i = 0; i < m_effectParams->n; i++) {
        if (m_particles[i]->active) {
            map.push_back(i);
        }
    }
    return map;
}

Mesh* ParticleEffect::getMesh(int i) {
    return m_effectParams->meshes->at(i);

}

ModelTexture* ParticleEffect::getModelTexture(int i) {
    return m_effectParams->textures->at(i);
}

ParticleEffect::EffectParams* ParticleEffect::createEffectParams(
    ParticleEffect::Type type,
    int n,
    float effectDuration,
    float particleDuration,
    float variance,
    float rate,
    float angle,
    float maxDist,
    bool loop,
    float magnitude,
    Vector<glm::vec3>* accelerators,
    Vector<Mesh *>* meshes,
    Vector<ModelTexture *>* textures
){
    ParticleEffect::EffectParams *effectParams = new ParticleEffect::EffectParams();
    effectParams->n = n;
    effectParams->effectDuration = effectDuration;
    effectParams->particleDuration = particleDuration;
    effectParams->variance = variance;
    effectParams->rate = rate;
    effectParams->angle = angle;
    effectParams->maxDist = maxDist;
    effectParams->loop = loop;
    effectParams->magnitude = magnitude;
    effectParams->accelerators = accelerators;
    effectParams->meshes = meshes;
    effectParams->textures = textures;
    return effectParams;
 }



int ParticleEffect::meshSelect(ParticleEffect::Effect effect, int i) {
    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT:
            return 0;
        default:
            return 0;
    }
}

int ParticleEffect::modelTextureSelect(ParticleEffect::Effect effect, int i) {
    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPLAT:
            return 0;
        default:
            return 0;
    }
}