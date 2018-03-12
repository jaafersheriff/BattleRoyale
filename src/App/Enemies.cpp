#include "Enemies.hpp"

#include "Lighting.hpp"
#include "Player.hpp"



namespace enemies {

namespace { // File static stuff

    // Constants
    const float k_basicSpeed = 3.0f;

    Vector<GameObject *> f_basicEnemies;

}

    void createBasic(const glm::vec3 & position) {    
        Mesh * mesh(Loader::getMesh("bunny.obj"));
        DiffuseShader * shader(RenderSystem::getShader<DiffuseShader>());
        ModelTexture modelTex(lighting::k_defAmbience, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f));
        bool toon(true);
        glm::vec3 scale(0.75f);
        unsigned int collisionWeight(5);
        float moveSpeed(3.0f);

        GameObject & obj(Scene::createGameObject());
        SpatialComponent & spatComp(Scene::addComponent<SpatialComponent>(obj, position, scale));
        NewtonianComponent & newtComp(Scene::addComponent<NewtonianComponent>(obj));
        GravityComponent & gravComp(Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj));
        BounderComponent & boundComp(CollisionSystem::addBounderFromMesh(obj, collisionWeight, *mesh, false, true, false));
        PathfindingComponent & pathComp(Scene::addComponent<PathfindingComponent>(obj, player::gameObject(), moveSpeed, false));
        DiffuseRenderComponent & renderComp = Scene::addComponent<DiffuseRenderComponent>(obj, spatComp, shader->pid, *mesh, modelTex, toon, glm::vec2(1.0f));   
        EnemyComponent & enemyComp(Scene::addComponent<EnemyComponent>(obj));
    
        f_basicEnemies.push_back(&obj);
    }

    void enablePathfinding() {
        for (GameObject * enemy : f_basicEnemies) {
            PathfindingComponent * comp(enemy->getComponentByType<PathfindingComponent>());
            if (!comp) {
                Scene::addComponent<PathfindingComponent>(*enemy, player::gameObject(), k_basicSpeed, false);
            }
        }
    }

    void disablePathfinding() {
        for (GameObject * enemy : f_basicEnemies) {
            PathfindingComponent * comp(enemy->getComponentByType<PathfindingComponent>());
            if (comp) {
                Scene::removeComponent(*comp);
            }
        }
    }

}