#include "ImGuiComponent.hpp"

void ImGuiComponent::addPane(std::string name, std::function<void(float)> fn) {
    panes[name].push_back(fn);
}

void ImGuiComponent::update(float dt) {
    for (auto & list : panes) {
        ImGui::Begin(list.first.c_str());
        for (auto fun : list.second) {
            fun(dt);
        }
        ImGui::End();
    }
}