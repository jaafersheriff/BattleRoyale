#include "StatsSystem.hpp"

#include "Scene/Scene.hpp"

const Vector<EnemyComponent *> & StatsSystem::s_enemyComponents(Scene::getComponents<EnemyComponent>());

void StatsSystem::update(float dt) {
    for (auto & comp : s_enemyComponents) {
        comp->update(dt);
    }
}