#include "Weapons.hpp"

#include "Lighting.hpp"
#include "Player.hpp"


namespace weapons {

    namespace grenade {

        const String k_meshName = "weapons/SodaCan.obj";
        const String k_texName = "weapons/SodaCan_Tex.png";
        const bool k_isToon = true;
        const glm::vec3 k_scale = glm::vec3(1.0f);
        const unsigned int k_weight = 1;
        const float k_speed = 20.0f; 
        const float k_damage = 100.0f;
        const float k_radius = 5.0f;

        void fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel) {
            const Mesh * mesh(Loader::getMesh(k_meshName));
            const Texture * tex(Loader::getTexture(k_texName));
            ModelTexture modelTex(tex, lighting::k_defAmbience, glm::vec3(1.0f), glm::vec3(1.0f));
            GameObject & obj(Scene::createGameObject());
            SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, k_scale));
            BounderComponent & bounderComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *mesh, false, true, false));
            NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj, true));
            GroundComponent & groundComp(Scene::addComponent<GroundComponent>(obj));
            Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj);
            newtComp.addVelocity(initDir * k_speed + srcVel);
            DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj,
                spatComp,
                RenderSystem::getShader<DiffuseShader>()->pid,
                *mesh,
                modelTex,
                k_isToon,
                glm::vec2(1.0f)
            ));
            ProjectileComponent & weaponComp(Scene::addComponentAs<GrenadeComponent, ProjectileComponent>(obj, k_damage, k_radius));
        }

        void firePlayer() {
            glm::vec3 initPos(player::camera->orientMatrix() * player::k_mainHandPosition + player::spatial->position());
            fire(initPos, player::camera->getLookDir(), player::spatial->effectiveVelocity());
        }

    }

    void destroyAllProjectiles() {        
        for (ProjectileComponent * comp : Scene::getComponents<ProjectileComponent>()) {
            Scene::destroyGameObject(comp->gameObject());
        }
        for (BlastComponent * comp : Scene::getComponents<BlastComponent>()) {
            Scene::destroyGameObject(comp->gameObject());
        }
    }

}