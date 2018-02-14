#include "SpatialSystem.hpp"



SpatialSystem::SpatialSystem(const std::vector<Component *> & components) :
    System(components)
{}

void SpatialSystem::update(float dt) {
    for (Component * comp : m_components) {
        //if (dynamic_cast<)
    }
}