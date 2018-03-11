#include "Weapons.hpp"

#include "Lighting.hpp"


namespace weapons {

namespace { // File static stuff

    Vector<GameObject *> f_basicProjectiles;

}

    void fireBasic(const glm::vec3 & initPos, const glm::vec3 & dir) {
        Mesh * mesh(Loader::getMesh("Hamburger.obj"));
        DiffuseShader * shader(RenderSystem::getShader<DiffuseShader>());
        Texture * tex(Loader::getTexture("Hamburger_BaseColor.png"));
        ModelTexture modelTex(tex, lighting::k_defAmbience, glm::vec3(1.0f), glm::vec3(1.0f));
        bool toon(true);
        glm::vec3 scale(0.05f);
        unsigned int collisionWeight(5);
        float speed(50.0f);

        GameObject & obj(Scene::createGameObject());
        SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, scale));
        BounderComponent & bounderComp(CollisionSystem::addBounderFromMesh(obj, collisionWeight, *mesh, false, true, false));
        NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj));
        Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj);
        newtComp.addVelocity(dir * speed);
        DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj, spatComp, shader->pid, *mesh, modelTex, true));
        ProjectileComponent & projectileComp(Scene::addComponent<ProjectileComponent>(obj));
    
        f_basicProjectiles.push_back(&obj);
    }

    void destroyAllProjectiles() {        
        for (GameObject * obj : f_basicProjectiles) {
            Scene::destroyGameObject(*obj);
        }
        f_basicProjectiles.clear();
    }

}