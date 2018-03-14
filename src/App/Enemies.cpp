#include "Enemies.hpp"

#include "Lighting.hpp"
#include "Player.hpp"



namespace enemies {

    namespace basic {

        const String k_meshName = "bunny.obj";
        const bool k_isToon = true;
        const glm::vec3 k_scale = glm::vec3(0.75f);
        const unsigned int k_weight = 5;
        const float k_moveSpeed = 3.0f;
        const float k_hp = 100.0f;

        Vector<GameObject *> list;

        void create(const glm::vec3 & position) {
            const Mesh * mesh(Loader::getMesh(k_meshName));
            const DiffuseShader * shader();
            ModelTexture modelTex(lighting::k_defAmbience, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f));
            GameObject & obj(Scene::createGameObject());
            SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, position, k_scale));
            NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj, false));
            GravityComponent & gravComp(Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj));
            BounderComponent & boundComp(CollisionSystem::addBounderFromMesh(obj, k_weight, *mesh, false, true, false));
            PathfindingComponent & pathComp(Scene::addComponent<PathfindingComponent>(obj, *player::gameObject, k_moveSpeed, false));
            DiffuseRenderComponent & renderComp = Scene::addComponent<DiffuseRenderComponent>(
                obj,
                spatComp,
                RenderSystem::getShader<DiffuseShader>()->pid,
                *mesh,
                modelTex,
                k_isToon,
                glm::vec2(1.0f)
            );
            HealthComponent & healthComp(Scene::addComponent<HealthComponent>(obj, k_hp));
            EnemyComponent & enemyComp(Scene::addComponentAs<BasicEnemyComponent, EnemyComponent>(obj));
    
            list.push_back(&obj);
        }

    }

    void enablePathfinding() {
        for (GameObject * enemy : basic::list) {
            PathfindingComponent * comp(enemy->getComponentByType<PathfindingComponent>());
            if (!comp) {
                Scene::addComponent<PathfindingComponent>(*enemy, *player::gameObject, basic::k_moveSpeed, false);
            }
        }
    }

    void disablePathfinding() {
        for (GameObject * enemy : basic::list) {
            PathfindingComponent * comp(enemy->getComponentByType<PathfindingComponent>());
            if (comp) {
                Scene::removeComponent(*comp);
            }
        }
    }

}