#include "GameLogicSystem.hpp"

void GameLogicSystem::update(float dt) {
    for (Component *cp : components) {
        cp->update(dt);
    }
}