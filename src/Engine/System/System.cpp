#include "System.hpp"



System::System(const std::vector<Component *> & components) :
    m_components(components)
{}

void System::update(float dt) {
    for (auto & c : m_components) {
        c->update(dt);
    }
}