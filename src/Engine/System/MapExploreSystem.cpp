#include "MapExploreSystem.hpp"

#include "Scene/Scene.hpp"

const Vector<MapExploreComponent *> & MapExploreSystem::s_mapexploreComponents(Scene::getComponents<MapExploreComponent>());

void MapExploreSystem::update(float dt) {
    for (auto & comp : s_mapexploreComponents) {
        comp->update(dt);
    }
}