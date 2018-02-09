#include "System.hpp"



System::System(const std::vector<Component *> & components) :
    components(components)
{}

void System::update(float dt) {
    for (auto & c : components) {
        c->update(dt);
    }
}