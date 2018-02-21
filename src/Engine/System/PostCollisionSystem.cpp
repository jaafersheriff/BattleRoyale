#include "PostCollisionSystem.hpp"

#include "Scene/Scene.hpp"



const std::vector<GroundComponent *> & PostCollisionSystem::s_groundComponents(Scene::getComponents<GroundComponent>());

void PostCollisionSystem::update(float dt) {
    for (auto & comp : s_groundComponents) {
        comp->update(dt);
    }
}