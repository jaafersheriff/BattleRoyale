#include "GameInterface.hpp"

#include "GameSystem.hpp"



float GameInterface::getAmbience() {
    return GameSystem::Lighting::k_defAmbience;
}

GameObject & GameInterface::getPlayer() {
    return *GameSystem::Player::gameObject;
}