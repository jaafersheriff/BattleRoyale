#include "EnemySystem.hpp"

#include "Scene/Scene.hpp"

const Vector<EnemyComponent *> & EnemySystem::s_enemyComponents(Scene::getComponents<EnemyComponent>());

void EnemySystem::update(float dt) {
    for (auto & comp : s_enemyComponents) {
        comp->update(dt);
    }
}