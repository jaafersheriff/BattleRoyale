#include "ParticleSystem.hpp"

#include "glm/gtc/random.hpp"
#include "glm/gtx/transform.hpp"

#include "Scene/Scene.hpp"
#include "Component/ParticleComponents/ParticleComponent.hpp"
#include "Loader/Loader.hpp"
#include "Component/SpatialComponents/SpatialComponent.hpp"
#include "Util/Util.hpp"



const int ParticleSystem::k_maxNParticles = 8192;
const int ParticleSystem::k_maxVariations = 20; // if changed, need to change vertex shader
const float ParticleSystem::k_minScaleFactor = 0.5f;
const float ParticleSystem::k_maxScaleFactor = 1.5f;

const Vector<ParticleComponent *> & ParticleSystem::s_particleComponents(Scene::getComponents<ParticleComponent>());
Vector<glm::mat4> ParticleSystem::m_variationMs = Vector<glm::mat4>(k_maxVariations);
Vector<glm::mat3> ParticleSystem::m_variationNs = Vector<glm::mat3>(k_maxVariations);

void ParticleSystem::init() {
    // Init variation matrices
    for (int i = 0; i < k_maxVariations; ++i) {
        glm::vec3 scaleVec(Util::random(k_minScaleFactor, k_maxScaleFactor));
        float angle(Util::random() * 2.0f * glm::pi<float>());
        glm::fvec3 & axis(glm::sphericalRand(1.0f));
        m_variationMs.push_back(Util::compositeTransform(scaleVec, glm::rotate(angle, axis)));
        m_variationNs.push_back(glm::mat3(m_variationMs.back()) * glm::mat3(glm::scale(glm::mat4(), 1.0f / scaleVec)));
    }
}

void ParticleSystem::update(float dt) {
    for (ParticleComponent * p : s_particleComponents) {
        p->update(dt);
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
    float minSpeed(10.0f);
    float maxSpeed(15.0f);
    float angle(glm::radians(15.0f));
    bool randomDistrib(true);
    auto initializer(UniquePtr<ConeParticleInitializer>::make(minSpeed, maxSpeed, angle, randomDistrib));
    auto updater(UniquePtr<GravityParticleUpdater>::make());
    Mesh & mesh(*Loader::getMesh("particles/Blood_Drop.obj"));
    ModelTexture modelTexture(Loader::getTexture("Blood_Drop_Tex.png"));
    int maxN(2000);
    float rate(1000.0f);
    float duration(1.0f);
    bool loop(true);
    float scale(1.0f);
    bool variation(false);
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
    Mesh & mesh(*Loader::getMesh("Hamburger.obj"));
    int maxN(500);
    float rate(4000.0f);
    float duration(0.25f);
    bool loop(false);
    float scale(0.05f);
    bool variation(true);
    bool fade(true);
    ModelTexture modelTexture(Loader::getTexture("Hamburger_BaseColor.png"));
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
/*
ParticleEffect::EffectParams* ParticleComponent::getEffectParams(ParticleEffect::Effect effect) {

    switch (effect) {
        case ParticleEffect::Effect::BLOOD_SPRAY: {
            ParticleEffect::Type type = ParticleEffect::Type::CONE;
            int n = 2000;
            float effectDuration = 5.0f;
            float particleDuration = 0.5f;
            int orientations = 0;
            bool randomDistribution = true; // Have a random distribution vs uniform distribution
            float variance = 0.1f;
            float rate = 1000.0f;
            float angle = glm::pi<float>()/24; // For Cones, must be limited to 0 -> pi
            bool loop = false;
            float magnitude = 30.0f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            //accelerators->push_back(glm::vec3(0.0f, -9.8f, 0.0f));
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return new ParticleEffect::EffectParams(type,n, effectDuration, particleDuration, orientations,
                randomDistribution, variance, rate, angle, loop, magnitude, accelerators, meshes, textures);
        }
        case ParticleEffect::Effect::WATER_FOUNTAIN: {
            ParticleEffect::Type type = ParticleEffect::Type::CONE;
            int n = 600;
            float effectDuration = 600.0f;
            float particleDuration = 3.0f;
            int orientations = 0;
            bool randomDistribution = true; // Have a random distribution vs uniform distribution
            float variance = 0.0f;
            float rate = 300.0f;
            float angle = glm::pi<float>() / 8; // For Cones, must be limited to 0 -> pi
            bool loop = true;
            float magnitude = 5.0f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            accelerators->push_back(glm::vec3(0.0f, -9.8f, 0.0f));
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return new ParticleEffect::EffectParams(type, n, effectDuration, particleDuration, orientations,
                randomDistribution, variance, rate, angle, loop, magnitude, accelerators, meshes, textures);
        }
        case ParticleEffect::Effect::SODA_GRENADE: {
            ParticleEffect::Type type = ParticleEffect::Type::SPHERE;
            int n = 500;
            float effectDuration = 0.5f;
            float particleDuration = 0.5f;
            int orientations = 20;
            bool randomDistribution = true; // Have a random distribution vs uniform distribution
            float variance = 0.2f;
            float rate = 0.0f;
            float angle = glm::pi<float>() / 4; // For Cones, must be limited to 0 -> pi
            bool loop = false;
            float magnitude = 40.0f;
            Vector<glm::vec3> * accelerators = new Vector<glm::vec3>();
            accelerators->push_back(glm::vec3(0.0f, -9.8f, 0.0f));
            Vector<Mesh *> *meshes = getMeshes(effect);
            Vector<ModelTexture*> * textures = getTextures(effect);
            return new ParticleEffect::EffectParams(type, n, effectDuration, particleDuration, orientations,
                randomDistribution, variance, rate, angle, loop, magnitude, accelerators, meshes, textures);
        }
    }

    return nullptr;
        
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
            meshes->push_back(Loader::getMesh("Hamburger.obj"));
            return meshes;
        }
    }

    return nullptr;
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
            Texture * tex(Loader::getTexture("Hamburger_BaseColor.png"));
            textures->push_back(new ModelTexture(tex));
            return textures;
        }
    }

    return nullptr;
}
    */