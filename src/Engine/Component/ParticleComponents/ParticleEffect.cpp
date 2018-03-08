#include "ParticleEffect.hpp"

ParticleEffect::ParticleEffect() :
    m_ep(NULL),
    m_origin(NULL),
    m_particles(Vector<Particle*>()),
    m_life(0.0f)
{
}


ParticleEffect::ParticleEffect(EffectParams *ep, const glm::vec3 & origin) :
    m_ep(ep),
    m_origin(origin),
    m_particles(generateParticles()),
    m_life(0.0f)
{
}

void ParticleEffect::update(float dt) {
    m_life += dt;
    float tData = m_life / m_ep->duration;
    if (m_life < m_ep->duration) {
        for (int i = 0; i < m_ep->n; i++) {
            switch (m_ep->type) {
                case ParticleEffect::Type::SPHERE:
                    sphereMove(m_particles[i], dt);
                    break;
            }
        }
    }
}

Vector<ParticleEffect::Particle*> ParticleEffect::generateParticles() {
    Vector<Particle*> vp = Vector<Particle*>();
    for (int i = 0; i < m_ep->n; i++) {
        Particle *p = new Particle();
        pinit(p, i, m_origin, 0, 0);
        vp.push_back(p);
        
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

void ParticleEffect::pinit(ParticleEffect::Particle *p, int i, glm::vec3 pos, int meshID, int modelTextureID) {
    p->i = i;
    p->position = pos;
    p->meshID = meshID;
    p->modelTextureID = modelTextureID;
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


void ParticleEffect::sphereMove(ParticleEffect::Particle* p, float dt) {
    float phi = glm::golden_ratio<float>();

    float z = 1 - (2 * float(p->i) / float(m_ep->n - 1));
    float radius = sqrt(1 - z * z);
    float theta = 2 * glm::pi<float>() * (2 - phi) * float(p->i);
    float default_speed = 10.f;
    p->position += float(default_speed * dt) * glm::vec3(radius * cos(theta), radius * sin(theta), z);
    //return p->position + float(default_speed * dt) * glm::vec3(radius * cos(theta), radius * sin(theta), z);
}

