#include "Weapons.hpp"

#include "Lighting.hpp"
#include "Player.hpp"


namespace weapons {

    namespace grenade {

        const String k_meshName = "Hamburger.obj";
        const String k_texName = "Hamburger_BaseColor.png";
        const bool k_isToon = true;
        const glm::vec3 k_scale = glm::vec3(1.0f);
        const unsigned int k_weight = 0; // TODO!!!!!!!!!!!
        const float k_speed = 30.0f; 

        Vector<GameObject *> list;

        void fire(const glm::vec3 & initPos, const glm::vec3 & initDir) {
            //const Mesh * mesh(Loader::getMesh("Hamburger.obj"));
            const Texture * tex(Loader::getTexture("Hamburger_BaseColor.png"));
            ModelTexture modelTex(tex, lighting::k_defAmbience, glm::vec3(1.0f), glm::vec3(1.0f));
            GameObject & obj(Scene::createGameObject());
            SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, initPos, k_scale));
            //BounderComponent & bounderComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *mesh, false, true, false));
            BounderComponent & bounderComp(Scene::addComponentAs<SphereBounderComponent, BounderComponent>(obj, k_weight, Sphere(glm::vec3(), 0.1f)));
            NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj));
            Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj);
            //newtComp.addVelocity(initDir * speed);
            /*DiffuseRenderComponent & renderComp(Scene::addComponent<DiffuseRenderComponent>(obj,
                spatComp,
                RenderSystem::getShader<DiffuseShader>()->pid,
                *mesh,
                modelTex,
                k_isToon,
                glm::vec2(1.0f)
            ));*/
    
            list.push_back(&obj);
        }

        void firePlayer() {
            glm::vec3 initPos(player::camera->orientMatrix() * player::k_mainHandPosition + player::spatial->position());
            /*auto pair(CollisionSystem::pick(Ray(player::spatial->position(), player::camera->getLookDir())));
            if (pair.second.is && &pair.first->gameObject() == player::gameObject) {
                initPos = pair.second.pos;
            }*/

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