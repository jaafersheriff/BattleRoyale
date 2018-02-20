#include "PostCollisionSystem.hpp"

std::vector<std::unique_ptr<GroundComponent>> PostCollisionSystem::s_groundComponents;

void PostCollisionSystem::update(float dt) {
    for (auto & comp : s_groundComponents) {
        comp->update(dt);
    }
}

void PostCollisionSystem::add(std::unique_ptr<Component> component) {
    if (dynamic_cast<GroundComponent *>(component.get()))
        s_groundComponents.emplace_back(static_cast<GroundComponent *>(component.release()));
    else
        assert(false);
}

void PostCollisionSystem::remove(Component * component) {
    if (dynamic_cast<GroundComponent *>(component)) {
        for (auto it(s_groundComponents.begin()); it != s_groundComponents.end(); ++it) {
            if (it->get() == component) {
                s_groundComponents.erase(it);
                break;
            }
        }
    }
}