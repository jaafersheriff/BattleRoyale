#include "ParticleEffect.hpp"

ParticleEffect::ParticleEffect() :
    m_ep(NULL),
    m_origin(NULL),
    m_euler(NULL),
    m_scale(NULL),
    m_particles(Vector<Particle>()),
    m_life(0.0f)
{
}


ParticleEffect::ParticleEffect(EffectParams *ep, const glm::vec3 & origin) :
    m_ep(ep),
    m_origin(origin),
    m_euler(glm::vec3(0)),
    m_scale(glm::vec3(0)),
    m_particles(generateParticles()),
    m_life(0.0f)
{
}

void ParticleEffect::update(float dt) {
    m_life += dt;
    float tData = m_life / m_ep->duration;
    if (m_life < m_ep->duration) {
        for (int i = 0; i < m_ep->n; i++) {
            m_particles[i].update(tData);
        }
    }
}

Vector<Particle> ParticleEffect::generateParticles() {
    Vector<Particle> vp = Vector<Particle>();
    for (int i = 0; i < m_ep->n; i++) {
        vp.emplace_back(m_ep->type, i, m_ep->n, m_origin);
        
    }
    return vp;
}

Mesh* ParticleEffect::getMesh(int i) {
    if (i < (int)m_ep->meshes->size()) {
        return m_ep->meshes->at(i);
    }
    else {
        return NULL;
    }

}

ModelTexture* ParticleEffect::getModelTexture(int i) {
    if (i < (int)m_ep->textures->size()) {
        return m_ep->textures->at(i);
    }
    else {
        return NULL;
    }
}

ParticleEffect::EffectParams* ParticleEffect::createEffectParams(
    ParticleEffect::Type type,
    int n,
    float duration,
    float variance,
    float rate,
    float angle,
    float maxDist,
    float mass,
    bool loop,
    glm::vec3* initVelocity,
    glm::vec3* forward,
    Vector<glm::vec3>* forceFactors,
    Vector<Mesh *>* meshes,
    Vector<ModelTexture *>* textures
){
    ParticleEffect::EffectParams *ep = new ParticleEffect::EffectParams();
    ep->n = n;
    ep->duration = duration;
    ep->variance = variance;
    ep->rate = rate;
    ep->angle = angle;
    ep->maxDist = maxDist;
    ep->mass = mass;
    ep->loop = loop;
    ep->initVelocity = initVelocity;
    ep->forward = forward;
    ep->forceFactors = forceFactors;
    ep->meshes = meshes;
    ep->textures = textures;
    return ep;
 }

