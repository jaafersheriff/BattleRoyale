#include "ParticleEffect.hpp"

ParticleEffect::ParticleEffect(EffectParams *effectParams, const glm::vec3 & offset) :
    m_effectParams(effectParams),
    m_anchor(glm::vec3(0)),
    m_offset(offset),
    m_direction(glm::vec3(0.0f, 1.0f, 0.0f)),
    m_velocity(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_particles(generateParticles()),
    m_activeParticlePositions(getActiveParticlePositions()),
    m_activeParticleOrientationIDs(getActiveParticleOrientationIDs()),
    m_activeMap(getActiveMap()),
    m_nextActivation(getNextActivation()),
    m_attenuationRate(1 / 60.0f),
    m_nextAttenuation(m_attenuationRate),
    m_life(0.0f)
{
}

ParticleEffect::ParticleEffect(EffectParams *effectParams, const glm::vec3 & offset, const glm::vec3 & direction) :
    m_effectParams(effectParams),
    m_anchor(glm::vec3(0)),
    m_offset(offset),
    m_direction(direction),
    m_velocity(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_particles(generateParticles()),
    m_activeParticlePositions(getActiveParticlePositions()),
    m_activeParticleOrientationIDs(getActiveParticleOrientationIDs()),
    m_activeMap(getActiveMap()),
    m_nextActivation(getNextActivation()),
    m_attenuationRate(1 / 60.0f),
    m_nextAttenuation(m_attenuationRate),
    m_life(0.0f)
{
}

ParticleEffect::ParticleEffect(EffectParams *effectParams, const glm::vec3 & offset, const glm::vec3 & direction, 
    const glm::vec3 & velocity) :
    m_effectParams(effectParams),
    m_anchor(glm::vec3(0)),
    m_offset(offset),
    m_direction(direction),
    m_velocity(velocity),
    m_particles(generateParticles()),
    m_activeParticlePositions(getActiveParticlePositions()),
    m_activeParticleOrientationIDs(getActiveParticleOrientationIDs()),
    m_activeMap(getActiveMap()),
    m_nextActivation(getNextActivation()),
    m_attenuationRate(1 / 60.0f),
    m_nextAttenuation(m_attenuationRate),
    m_life(0.0f)
{
}

void ParticleEffect::update(float dt, glm::vec3 anchor, glm::vec3 direction) {
    m_anchor = anchor;
    m_direction = direction;
    
    update(dt);
}

void ParticleEffect::update(float dt, glm::vec3 anchor) {
    m_anchor = anchor;
    update(dt);
}

void ParticleEffect::update(float dt) {
    m_life += dt;

    if (m_life < m_effectParams->effectDuration) {
        updateActiveParticles(dt);
        int attenuations = getAttenuations(dt);
        float factor = 1.0f;
        if (attenuations != 0) {
            factor= pow(m_effectParams->attenuation, attenuations);
        }
        for (int i = 0; i < (int)m_activeParticlePositions->size(); i++) {
            Particle *p = m_particles[m_activeMap[i]];
            updatePosition(p, dt, factor);
            m_activeParticlePositions->at(i) = p->position;
            p->life += dt;

        }
    }
    else if (m_effectParams->loop) {
        if (m_effectParams->rate == 0) {
            m_life = 0.0f;
            m_particles = generateParticles();
            m_activeParticlePositions = getActiveParticlePositions();
            m_activeParticleOrientationIDs = getActiveParticleOrientationIDs();
            m_activeMap = getActiveMap();
            m_nextActivation = getNextActivation();
            m_nextAttenuation = 0.0f;
        }
        else {
            m_effectParams->effectDuration += m_life;
        }
        update(dt);
    }
}



void ParticleEffect::updateActiveParticles(float dt) {
    
    if (m_effectParams->rate != 0.0f) {
        while ((int)m_activeParticlePositions->size() > 0) {
            Particle *p = m_particles[m_activeMap[0]];
            if (p->life > m_effectParams->particleDuration) {
                removeActiveParticle(0);
            }
            else {
                break;
            }
        }
        // Activate new particles based on rate. Cap at n limit and Remove the oldest Particle
        while (m_life> m_nextActivation) {
            if (m_activeParticlePositions->size() == m_effectParams->n) {
                int pid = m_activeMap[0];
                removeActiveParticle(0);
                m_particles[pid] = makeParticle(pid);
                addActiveParticle(pid);
            }
            else {
                int pid;
                if (m_activeMap.size() == 0) {
                    pid = 0;
                }
                else {
                    pid = m_activeMap.back() + 1;
                    if (pid == m_effectParams->n) {
                        pid = 0;
                    }
                }
                m_particles[pid] = makeParticle(pid);
                addActiveParticle(pid);
            }
            m_nextActivation += (1.0f / m_effectParams->rate);
        }

    }
}

void ParticleEffect::updatePosition(Particle *p, float dt, float attenuationFactor) {
    p->velocity *= attenuationFactor;
    for (int i = 0; i < (int)m_effectParams->accelerators->size(); i++) {
        p->velocity += m_effectParams->accelerators->at(i) * dt;
    }
    p->position += p->velocity * dt; 
}

int ParticleEffect::getAttenuations(float dt) {
    if (m_effectParams->attenuation == 1.0f) {
        return 0;
    }
    else {
        int attenuations = 0;
        while (m_life > m_nextAttenuation) {
            attenuations += 1;
            m_nextAttenuation += m_attenuationRate;
        }
        return attenuations;
    }
}

 //Remove from specific location
void ParticleEffect::removeActiveParticle(int i) {
    m_activeMap.erase(m_activeMap.begin() + i);
    m_activeParticlePositions->erase(m_activeParticlePositions->begin() + i);
    m_activeParticleOrientationIDs->erase(m_activeParticleOrientationIDs->begin() + i);
}

//Will always add to end of vector
void ParticleEffect::addActiveParticle(int i) {
    m_activeMap.push_back(i);
    m_activeParticlePositions->push_back(m_particles[i]->position);
    m_activeParticleOrientationIDs->push_back(m_particles[i]->orientationID);
}

void ParticleEffect::sphereMotion(Particle* p) {
    if (!m_effectParams->randomDistribution) {
        float phi = glm::golden_ratio<float>();
        float z = 1 - (2 * float(p->i) / float(m_effectParams->n - 1));
        float radius = sqrt(1 - z * z);
        float theta = 2 * glm::pi<float>() * (2 - phi) * float(p->i);
        p->velocity = glm::vec3(radius * cos(theta), radius * sin(theta), z);
    }
    else {
        p->velocity = normalize(glm::ballRand(1.0f));
    }
}

void ParticleEffect::diskMotion(Particle* p) {
    if (!m_effectParams->randomDistribution) {
        float theta = m_effectParams->angle * (p->i / (float)m_effectParams->n);
        p->velocity = glm::vec3(cos(theta), 0.0f, sin(theta));
    }
    else {
        float theta = getRandom(-1 * m_effectParams->angle / 2.0f, m_effectParams->angle / 2);
        p->velocity = glm::vec3(cos(theta), 0.0f, sin(theta));
    }
}

void ParticleEffect::coneMotion(Particle* p) {
    
    glm::vec3 normalizedDirection = normalize(m_direction);
    glm::vec3 u = getU(normalizedDirection);
    glm::vec3 v = glm::cross(u, normalizedDirection);
    float angle = m_effectParams->angle;
    float phi = getRandom(-1 * glm::pi<float>(), glm::pi<float>());
    float z = getRandom(cos(angle), 1);
    float theta = acosf(z);
    p->velocity = sin(theta) * (cos(phi) * u + sin(phi) * v) + cos(theta) * normalizedDirection;
}

glm::vec3 ParticleEffect::getU(glm::vec3 directionNorm) {
    glm::vec3 randVec = glm::ballRand(1.0f);
    glm::vec3 u = normalize(cross(directionNorm, randVec));
    while (u == glm::vec3(0.0f)) {
        randVec = glm::ballRand(1.0f);
        u = normalize(cross(directionNorm, randVec));
    }
    return u;
}


float ParticleEffect::getRandom(float low, float high) {
    return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
}

Vector<ParticleEffect::Particle*> ParticleEffect::generateParticles() {
    Vector<Particle*> vp = Vector<Particle*>();
    for (int i = 0; i < m_effectParams->n; i++) {
        Particle *p = makeParticle(i);
        vp.push_back(p);
    }
    return vp;
}

// This and the inits need to eventually be cleaned up
ParticleEffect::Particle* ParticleEffect::makeParticle(int i) {
    Particle *p = new Particle();
    
    initParticle(p, i, 0, 0);

    //Determine if active/inactive
    if (m_effectParams->rate == 0.0f) {
        p->active = true;
    }
    else {
        p->active = false;
    }

    // Get Orientation ID
    if (m_effectParams->orientations == 0) {
        p->orientationID = 0;
    }
    else {
        p->orientationID = p->i % m_effectParams->orientations;
    }
    

    initPosition(p);

    initVelocity(p);

    return p;
}

void ParticleEffect::initParticle(Particle *p, int i, int meshID, int modelTextureID) {
    p->i = i;
    p->meshID = meshID;
    p->modelTextureID = modelTextureID;
    p->life = 0.0f;
}

void ParticleEffect::initVelocity(Particle *p) {
    switch (m_effectParams->type) {
        case ParticleEffect::SPHERE : {
            sphereMotion(p);
            break;
        }
        case ParticleEffect::DISK : {
            diskMotion(p);
            break;
        }
        case ParticleEffect::CONE: {
            coneMotion(p);
            break;
        }
    }

    //Add random variance if specified
    if (m_effectParams->variance != 0.0f) {
        glm::vec3 randVec = m_effectParams->variance * normalize(glm::ballRand(1.0f));
        p->velocity += randVec;
    }

    if (m_effectParams->randomDistribution) {
        p->velocity *= getRandom(0.25f, 1.0f);
    }

    //set speed
    p->velocity *= m_effectParams->magnitude;
}

void ParticleEffect::initPosition(Particle *p) {
    p->position = m_anchor + m_offset + m_life * m_velocity;
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

Vector<int> * ParticleEffect::getActiveParticleOrientationIDs() {
    Vector<int> *orientationIDs = new Vector<int>();
    for (int i = 0; i < m_effectParams->n; i++) {
        if (m_particles[i]->active) {
            orientationIDs->push_back(m_particles[i]->orientationID);
        }
    }
    return orientationIDs;
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

float ParticleEffect::getNextActivation() {
    if (m_effectParams->rate == 0.0f) {
        return 0.0f;
    }
    else {
        return 1.0f / m_effectParams->rate;
    }
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
    int orientations,
    bool randomDistribution,
    float variance,
    float rate,
    float angle,
    bool loop,
    float magnitude,
    float attenuation,
    Vector<glm::vec3>* accelerators,
    Vector<Mesh *>* meshes,
    Vector<ModelTexture *>* textures
){
    ParticleEffect::EffectParams *effectParams = new ParticleEffect::EffectParams();
    effectParams->type = type;
    effectParams->n = n;
    effectParams->effectDuration = effectDuration;
    effectParams->particleDuration = particleDuration;
    effectParams->orientations = orientations;
    effectParams->randomDistribution = randomDistribution;
    effectParams->variance = variance;
    effectParams->rate = rate;
    effectParams->angle = angle;
    effectParams->loop = loop;
    effectParams->magnitude = magnitude;
    effectParams->attenuation = attenuation;
    effectParams->accelerators = accelerators;
    effectParams->meshes = meshes;
    effectParams->textures = textures;
    return effectParams;
 }