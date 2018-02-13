#include "ImGuiComponent.hpp"

#include <iostream>
void ImGuiComponent::update(float dt) {
    for (auto iter : panes) {
        std::cout << iter.first.c_str() << std::endl;
        // TODO imgui
    }
}