#include "EnemyComponent.hpp"

#include "Scene/Scene.hpp"

void EnemyComponent::update(float dt) {
    if(this->hp < 0.f) {
        Scene::destroyGameObject(this->gameObject());
    }
} 
