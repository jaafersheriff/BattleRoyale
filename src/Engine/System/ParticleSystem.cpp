#include "ParticleSystem.hpp"

#include "glm/gtc/random.hpp"
#include "glm/gtx/transform.hpp"

#include "Scene/Scene.hpp"
#include "Component/ParticleComponents/ParticleComponent.hpp"
#include "Component/ParticleComponents/ParticleAssasinComponent.hpp"
#include "Loader/Loader.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Util/Util.hpp"



const int ParticleSystem::k_maxNParticles = 8192;
const int ParticleSystem::k_maxVariations = 20; // if changed, need to change diffuse shader and shadow shader
const float ParticleSystem::k_minScaleFactor = 0.5f;
const float ParticleSystem::k_maxScaleFactor = 1.5f;

const Vector<ParticleComponent *> & ParticleSystem::s_particleComponents(Scene::getComponents<ParticleComponent>());
const Vector<ParticleAssasinComponent *> & ParticleSystem::s_particleAssasinComponents(Scene::getComponents<ParticleAssasinComponent>());
Vector<glm::mat4> ParticleSystem::s_variationMs = Vector<glm::mat4>(k_maxVariations);
Vector<glm::mat3> ParticleSystem::s_variationNs = Vector<glm::mat3>(k_maxVariations);

void ParticleSystem::init() {
    // Init variation matrices
    for (int i = 0; i < k_maxVariations; ++i) {
        glm::vec3 scaleVec(Util::random(k_minScaleFactor, k_maxScaleFactor));
        float angle(Util::random() * 2.0f * glm::pi<float>());
        const glm::fvec3 & axis(glm::sphericalRand(1.0f));
        s_variationMs[i] = Util::compositeTransform(scaleVec, glm::rotate(angle, axis));
        s_variationNs[i] = glm::mat3(s_variationMs.back()) * glm::mat3(glm::scale(glm::mat4(), 1.0f / scaleVec));
    }
}

void ParticleSystem::update(float dt) {
    for (ParticleComponent * comp : s_particleComponents) {
        comp->update(dt);
    }
    for (ParticleAssasinComponent * comp : s_particleAssasinComponents) {
        comp->update(dt);
    }
}

ParticleComponent & ParticleSystem::addBodyExplosionPC(SpatialComponent & spatial) {   
    float minSpeed(0.1f);
    float maxSpeed(3.0f);
    bool randomDistrib(true); 
    auto initializer(UniquePtr<SphereParticleInitializer>::make(minSpeed, maxSpeed, randomDistrib));
    auto updater(UniquePtr<GravityParticleUpdater>::make());
    Mesh & mesh(*Loader::getMesh("particles/Gore_Chunk.obj"));
    ModelTexture modelTexture(Loader::getTexture("particles/Gore_Chunk_Tex.png"));
    int maxN(100);
    float rate(0.0f);
    float duration(1.0f);
    bool loop(false);
    float scale(1.0f);
    bool variation(true);
    bool fade(true);
    return Scene::addComponent<ParticleComponent>(spatial.gameObject(),
        std::move(initializer),
        std::move(updater),
        maxN,
        rate,
        duration,
        loop,
        mesh,
        scale,
        modelTexture,
        spatial,
        variation,
        fade
    );
}

ParticleComponent & ParticleSystem::addWaterFountainPC(SpatialComponent & spatial) {
    float minSpeed(2.0f);
    float maxSpeed(5.0f);
    float angle(glm::radians(40.0f));
    bool randomDistrib(true);
    auto initializer(UniquePtr<ConeParticleInitializer>::make(minSpeed, maxSpeed, angle, randomDistrib));
    auto updater(UniquePtr<GravityParticleUpdater>::make());
    Mesh & mesh(*Loader::getMesh("particles/Blood_Drop.obj"));
    ModelTexture modelTexture(Loader::getTexture("particles/Blood_Drop_Tex.png"));
    int maxN(2000);
    float rate(1000.0f);
    float duration(2.0f);
    bool loop(true);
    float scale(1.0f);
    bool variation(true);
    bool fade(true);
    return Scene::addComponent<ParticleComponent>(spatial.gameObject(),
        std::move(initializer),
        std::move(updater),
        maxN,
        rate,
        duration,
        loop,
        mesh,
        scale,
        modelTexture,
        spatial,
        variation,
        fade
    );
}

ParticleComponent & ParticleSystem::addSodaGrenadePC(SpatialComponent & spatial) {
    float minSpeed(20.0f);
    float maxSpeed(30.0f);
    bool randomDistrib(true);
    auto initializer(UniquePtr<SphereParticleInitializer>::make(minSpeed, maxSpeed, randomDistrib));
    auto updater(UniquePtr<AttenuationParticleUpdater>::make(5.0f));
    Mesh & mesh(*Loader::getMesh("particles/Soda_2.obj"));
    int maxN(500);
    float rate(4000.0f);
    float duration(0.25f);
    bool loop(false);
    float scale(1.0f);
    bool variation(true);
    bool fade(true);
    ModelTexture modelTexture(Loader::getTexture("particles/Soda_Tex.png"));
    return Scene::addComponent<ParticleComponent>(spatial.gameObject(),
        std::move(initializer),
        std::move(updater),
        maxN,
        rate,
        duration,
        loop,
        mesh,
        scale,
        modelTexture,
        spatial,
        variation,
        fade
    );
}

ParticleComponent & ParticleSystem::addSrirachaPC(SpatialComponent & spatial) {
    float minSpeed(2.0f);
    float maxSpeed(5.0f);
    float angle(glm::radians(40.0f));
    bool randomDistrib(true);
    auto initializer(UniquePtr<ConeParticleInitializer>::make(minSpeed, maxSpeed, angle, randomDistrib));
    auto updater(UniquePtr<GravityParticleUpdater>::make());
    Mesh & mesh(*Loader::getMesh("particles/Blood_Drop.obj"));
    ModelTexture modelTexture(Loader::getTexture("particles/Blood_Drop_Tex.png"));
    int maxN(2000);
    float rate(1000.0f);
    float duration(2.0f);
    bool loop(true);
    float scale(1.0f);
    bool variation(true);
    bool fade(true);
    return Scene::addComponent<ParticleComponent>(spatial.gameObject(),
        std::move(initializer),
        std::move(updater),
        maxN,
        rate,
        duration,
        loop,
        mesh,
        scale,
        modelTexture,
        spatial,
        variation,
        fade
    );
}