#include "ParticleComponent.hpp"

#include "glm/gtc/random.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"

#include "Scene/Scene.hpp"
#include "System/ParticleSystem.hpp"
#include "Util/Util.hpp"
#include "System/SpatialSystem.hpp"
#include "Loader/Loader.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"



Particle::Particle(
    const glm::vec3 & position,
    const glm::vec3 & velocity,
    unsigned int matrixI
) :
    position(position),
    matrixI(matrixI),
    velocity(velocity),
    life(0.0f)
{}



void SphereParticleInitializer::initialize(Particle & p, unsigned int i, unsigned int n) {
    if (m_randomDistribution) {
        p.velocity = glm::sphericalRand(1.0f);
    }
    else {
        constexpr float phi = glm::golden_ratio<float>();
        float z = 1.0f - (2.0f * float(i) / float(n - 1));
        float radius = std::sqrt(1 - z * z);
        float theta = 2.0f * glm::pi<float>() * (2.0f - phi) * float(i);
        p.velocity = glm::vec3(radius * std::cos(theta), radius * std::sin(theta), z);
    }
    p.velocity *= Util::random(m_minSpeed, m_maxSpeed);
}



void ConeParticleInitializer::initialize(Particle & p, unsigned int i, unsigned int n) {
    if (m_randomDistribution) {
        float theta = Util::random() * 2.0f * glm::pi<float>();
        float phi = Util::random() * m_angle;
        p.velocity = Util::sphericalToCartesian(1.0f, theta, phi);
        p.velocity.z *= -1.0f;
    }
    else {
        constexpr float phi = glm::golden_ratio<float>();
        float radius = std::sqrt(float(i) / float(n - 1));
        float theta = 2.0f * glm::pi<float>() * (2.0f - phi) * float(i);
        float z = 1.0f / std::tan(m_angle);
        p.velocity = glm::normalize(glm::vec3(radius * std::cos(theta), radius * std::sin(theta), -z));
    }
    p.velocity *= Util::random(m_minSpeed, m_maxSpeed);
}



void DiskParticleInitializer::initialize(Particle & p, unsigned int i, unsigned int n) {
    if (m_randomDistribution) {
        float theta = Util::random(-m_angle * 0.5f, m_angle * 0.5f);
        p.velocity = glm::vec3(cos(theta), sin(theta), 0.0f);
    }
    else {
        float theta = m_angle * float(i) / float(n - 1);
        p.velocity = glm::vec3(cos(theta), sin(theta), 0.0f);
    }
    p.velocity *= Util::random(m_minSpeed, m_maxSpeed);
}



void LinearParticleUpdater::update(Vector<Particle> & particles, float dt) {
    for (Particle & p : particles) {
        p.position += p.velocity * dt;
    }
}



void GravityParticleUpdater::update(Vector<Particle> & particles, float dt) {
    for (Particle & p : particles) {
        glm::vec3 newVelocity(p.velocity + SpatialSystem::gravity() * dt);
        p.position += 0.5f * dt * (p.velocity + newVelocity);
        p.velocity = newVelocity;
    }
}



void AttenuationParticleUpdater::update(Vector<Particle> & particles, float dt) {
    for (Particle & p : particles) {
        p.position += p.velocity * dt;
        p.velocity *= std::exp(-m_attenuation * dt);
    }
}



ParticleComponent::ParticleComponent(GameObject & gameObject,
    UniquePtr<ParticleInitializer> && initializer,
    UniquePtr<ParticleUpdater> && updater,
    int maxN,
    float rate,
    float duration,
    bool loop,
    const Mesh & mesh,
    float scale,
    const ModelTexture & modelTexture,
    const SpatialComponent & anchor,
    bool variation,
    bool fade
) :
    Component(gameObject),
    m_initializer(std::move(initializer)),
    m_updater(std::move(updater)),
    m_maxN(glm::min(maxN, ParticleSystem::k_maxNParticles)),
    m_rate(rate),
    m_duration(duration),
    m_loop(loop),
    m_mesh(mesh),
    m_scale(scale),
    m_modelTexture(modelTexture),
    m_anchor(anchor),
    m_variation(variation),
    m_fade(fade),

    m_particles(),   
    m_timeAccum(0.0f),
    m_runningParticleID(0),
    m_firstSpawn(true)
{}

void ParticleComponent::init() {

}   

void ParticleComponent::update(float dt) {
    // Age particles
    for (Particle & p : m_particles) {
        p.life += dt;
    }

    // Remove all expired particles
    auto it(m_particles.begin());
    while (it != m_particles.end() && it->life > m_duration) ++it;
    m_particles.erase(m_particles.begin(), it);

    // Update active particles
    m_updater->update(m_particles, dt);

    // Don't create new particles if this is not the first spawn
    if (!m_loop && !m_firstSpawn) {
        if (m_particles.empty()) {
            Scene::removeComponent(*this);
        }
        return;
    }

    // Determine number of particles to create
    int nToSpawn;
    if (m_rate == 0.0f) {
        nToSpawn = m_maxN - int(m_particles.size());
    }
    else {
        m_timeAccum += dt;
        nToSpawn = int(std::floor(dt * m_rate));
        m_timeAccum -= float(nToSpawn) / m_rate;
    }

    // Cap number of particles to create
    if (nToSpawn > m_maxN) nToSpawn = m_maxN;

    // If don't have room for new particles, make it
    int nToFree(m_loop ? nToSpawn + int(m_particles.size()) - m_maxN : 0);
    if (nToFree > 0) {
        m_particles.erase(m_particles.begin(), m_particles.begin() + nToFree);
    }

    // Create new particles
    for (int i(0); i < nToSpawn; ++i) {
        m_particles.emplace_back(glm::vec3(), glm::vec3(), m_runningParticleID % ParticleSystem::k_maxVariations);
        Particle & p(m_particles.back());
        m_initializer->initialize(p, m_runningParticleID, m_maxN);
        // move and orient relative to anchor
        p.position += m_anchor.position();
        p.velocity = m_anchor.orientMatrix() * p.velocity;
        //TODO: p.velocity += m_anchor.effectiveVelocity();
        if (++m_runningParticleID >= unsigned int(m_maxN)) {
            m_runningParticleID = 0;
            m_firstSpawn = false;
            if (!m_loop) {
                break;
            }
        }
    }
}