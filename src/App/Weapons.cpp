#include "Weapons.hpp"

#include "Lighting.hpp"
#include "Player.hpp"


namespace weapons {

    namespace grenade {

        const Mesh * mesh = Loader::getMesh("Hamburger.obj");
        const ModelTexture modelTex = ModelTexture(Loader::getTexture("Hamburger_BaseColor.png"), lighting::k_defAmbience, glm::vec3(1.0f), glm::vec3(1.0f));
        const bool isToon = true;
        const glm::vec3 scale = glm::vec3(0.05f);
        const unsigned int weight = 1;
        const float speed = 30.0f; 

        Vector<GameObject *> list;   

        void fire(const glm::vec3 & initPos, const glm::vec3 & initDir) {
            GameObject & obj(Scene::createGameObject());
            SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, scale));
            BounderComponent & bounderComp(CollisionSystem::addBounderFromMesh(obj, weight, *mesh, false, true, false));
            NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj));
            Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj);
            newtComp.addVelocity(initDir * speed);
            DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj,
                spatComp,
                RenderSystem::getShader<DiffuseShader>()->pid,
                *mesh,
                modelTex,
                isToon,
                glm::vec2(1.0f)
            ));
    
            list.push_back(&obj);
        }

        void firePlayer() {
            glm::vec3 initPos(player::spatial->position());
            auto pair(CollisionSystem::pick(Ray(player::spatial->position(), player::camera->getLookDir())));
            if (pair.second.is && &pair.first->gameObject() == player::gameObject) {
                initPos = pair.second.pos;
            }

            fire(initPos, player::camera->getLookDir());
        }

    }

    void destroyAllProjectiles() {        
        for (GameObject * obj : grenade::list) {
            Scene::destroyGameObject(*obj);
        }
        grenade::list.clear();
    }

}