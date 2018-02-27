#include "PostCollisionSystem.hpp"

#include "Scene/Scene.hpp"



const Vector<GroundComponent *> & PostCollisionSystem::s_groundComponents(Scene::getComponents<GroundComponent>());

void PostCollisionSystem::update(float dt) {
    for (auto & comp : s_groundComponents) {
        comp->update(dt);
    }
    // this is here and not in SpatialSystem because this needs to happend after collision
    for (auto & comp : Scene::getComponents<SpatialComponent>()) {
        comp->update(dt);
    }
}